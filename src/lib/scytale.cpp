/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Product name: redemption, a FLOSS RDP proxy
   Copyright (C) Wallix 2017
   Author(s): Christophe Grosjean, Jonathan Poelen

*/

#include "lib/scytale.hpp"
#include "transport/crypto_transport.hpp"
#include "transport/mwrm_reader.hpp"
#include "capture/mwrm3.hpp"
#include "test_only/lcg_random.hpp"
#include "utils/fileutils.hpp"
#include "utils/genfstat.hpp"
#include "utils/c_interface.hpp"
#include "utils/sugar/algostring.hpp"
#include "std17/charconv.hpp"

#include "main/version.hpp"

#include <memory>

#define CHECK_NOTHROW_R(expr, return_err, error_ctx, errid) \
    do {                                                    \
        try { expr; }                                       \
        catch (Error const& err) {                          \
            EXIT_ON_ERROR(err);                             \
            error_ctx.set_error(err);                       \
            return return_err;                              \
        }                                                   \
        catch (...) {                                       \
            EXIT_ON_EXCEPTION();                            \
            error_ctx.set_error(Error{errid});              \
            return return_err;                              \
        }                                                   \
    } while (0)

#define CHECK_NOTHROW(expr, errid) CHECK_NOTHROW_R(expr, -1, handle->error_ctx, errid)
#define CREATE_HANDLE(construct) [&]()->decltype(new construct){ \
    CHECK_NOTHROW_R(                                             \
        auto handle = new (std::nothrow) construct; /*NOLINT*/   \
        return handle,                                           \
        nullptr,                                                 \
        ScytaleErrorContext(),                                   \
        ERR_MEMORY_ALLOCATION_FAILED                             \
    );                                                           \
}()


extern "C"
{

struct CryptoContextWrapper
{
    CryptoContext cctx;

    CryptoContextWrapper(
        get_hmac_key_prototype * hmac_fn, get_trace_key_prototype * trace_fn,
        bool with_encryption, bool with_checksum,
        bool old_encryption_scheme, bool one_shot_encryption_scheme,
        std::string_view master_derivator)
    {
        cctx.set_get_hmac_key_cb(hmac_fn);
        cctx.set_get_trace_key_cb(trace_fn);
        cctx.set_trace_type(
            with_encryption
            ? TraceType::cryptofile
            : with_checksum
                ? TraceType::localfile_hashed
                : TraceType::localfile);
        cctx.old_encryption_scheme = old_encryption_scheme;
        cctx.one_shot_encryption_scheme = one_shot_encryption_scheme;
        cctx.set_master_derivator(master_derivator);
    }
};

struct ScytaleErrorContext
{
    ScytaleErrorContext() noexcept
    : error(NO_ERROR)
    , msg{}
    {}

    char const * message() noexcept
    {
        if (this->error.errnum) {
            std::snprintf(this->msg, sizeof(msg), "%s, errno = %d: %s",
                this->error.errmsg().c_str(), this->error.errnum, strerror(this->error.errnum));
            this->msg[sizeof(this->msg)-1] = 0;
            return this->msg;
        }

        return this->error.errmsg();
    }

    static char const * handle_get_error_message() noexcept
    {
        return "Handle is nullptr";
    }

    void set_error(Error const & err) noexcept
    {
        this->error = err;
    }

private:
    Error error;
    char msg[256];
};

namespace
{
    struct HashHex
    {
        HashHex() noexcept
        {
            memset(this->hashhex, '0', sizeof(this->hashhex)-1); // NOLINT(bugprone-suspicious-memset-usage)
            this->hashhex[sizeof(this->hashhex)-1] = 0;
        }

        [[nodiscard]] char const* c_str() const noexcept
        {
            return this->hashhex;
        }

        HashHexArray& data() noexcept
        {
            return this->hashhex;
        }

    private:
        HashHexArray hashhex;
    };

    struct ScytaleRandomWrapper
    {
        enum RandomType : bool { LCG, UDEV };

        Random * rnd;

        ScytaleRandomWrapper(RandomType rnd_type)
        {
            switch (rnd_type) {
                case LCG: rnd = new (&u.lcg) LCGRandom(0); break;
                case UDEV: rnd = new (&u.udev) UdevRandom(); break;
            }
        }

        ~ScytaleRandomWrapper()
        {
            rnd->~Random();
        }

    private:
        union U {
            LCGRandom lcg; /* for reproductible tests */
            UdevRandom udev;
            char dummy;
            U() : dummy() {}
            ~U() {} /*NOLINT*/
        } u;
    };
} // namespace

struct ScytaleWriterHandle
{
    ScytaleWriterHandle(
        ScytaleRandomWrapper::RandomType random_type,
        bool with_encryption, bool with_checksum,
        get_hmac_key_prototype * hmac_fn, get_trace_key_prototype * trace_fn,
        int old_encryption_scheme , int one_shot_encryption_scheme,
        char const * master_derivator)
    : random_wrapper(random_type)
    , cctxw(hmac_fn, trace_fn, with_encryption, with_checksum,
            old_encryption_scheme, one_shot_encryption_scheme,
            master_derivator)
    , out_crypto_transport(cctxw.cctx, *random_wrapper.rnd, fstat)
    {}

private:
    ScytaleRandomWrapper random_wrapper;
    Fstat fstat;

    CryptoContextWrapper cctxw;

public:
    HashHex qhashhex;
    HashHex fhashhex;

    OutCryptoTransport out_crypto_transport;
    ScytaleErrorContext error_ctx;
};


struct ScytaleReaderHandle
{
    HashHex qhashhex;
    HashHex fhashhex;

    ScytaleReaderHandle(
        InCryptoTransport::EncryptionMode encryption,
        get_hmac_key_prototype * hmac_fn, get_trace_key_prototype * trace_fn,
        int old_encryption_scheme, int one_shot_encryption_scheme,
        char const * master_derivator)
    : cctxw(hmac_fn, trace_fn,
            false /* unused for reading */, false /* unused for reading */,
            old_encryption_scheme, one_shot_encryption_scheme,
            master_derivator)
    , in_crypto_transport(cctxw.cctx, encryption, this->fstat)
    {}

    CryptoContextWrapper cctxw;
    Fstat fstat;

    InCryptoTransport in_crypto_transport;
    ScytaleErrorContext error_ctx;
};


using HashArray = uint8_t[MD_HASH::DIGEST_LENGTH];
static_assert(sizeof(HashArray) * 2 + 1 == sizeof(HashHexArray));

namespace
{
    inline void hash_to_hashhex(HashArray const & hash, HashHexArray& hashhex) noexcept {
        char const * t = "0123456789ABCDEF";
        static_assert(sizeof(hash) * 2 + 1 == sizeof(HashHexArray));
        auto phex = hashhex;
        for (uint8_t c : hash) {
            *phex++ = t[c >> 4];
            *phex++ = t[c & 0xf];
        }
        *phex = '\0';
    }

    inline void chars_to_hash_impl(bytes_view hashhex, HashArray& hash)
    {
        assert(hashhex.size() == sizeof(HashHexArray) - 1);
        auto phex = hash;
        for (size_t i = 0 ; i < sizeof(HashHexArray) - 1; i += 2) {
            auto c1 = hashhex[i];
            auto c2 = hashhex[i+1];
            *phex++ = ((0xF & (c1 < 'A'? c1 - '0' : c1 < 'a' ? c1 - 'A' : c1 - 'a')) << 4)
                    |  (0xF & (c2 < 'A'? c2 - '0' : c2 < 'a' ? c2 - 'A' : c2 - 'a'));
        }
    }

    inline void hashex_to_hash(HashHexArray const & hashhex, HashArray& hash) noexcept {
        // Undefined Behavior if hashhex is not a valid input hex key
        static_assert(sizeof(HashArray) * 2 + 1 == sizeof(HashHexArray));
        auto av = make_array_view(hashhex);
        chars_to_hash_impl(av.drop_back(1), hash);
    }

    [[nodiscard]]
    inline bool chars_to_hash(char const* hashhex, HashArray& hash) noexcept {
        auto len = strlen(hashhex);
        if (len != sizeof(HashHexArray)-1u) {
            return false;
        }
        chars_to_hash_impl(bytes_view(hashhex, len), hash);
        return true;
    }
} // namespace



const char* scytale_version() {
    return VERSION;
}


/// \return HashHexArray
char const * scytale_writer_get_qhashhex(ScytaleWriterHandle * handle) {
    SCOPED_TRACE;
    CHECK_HANDLE_R(handle, "");
    return handle->qhashhex.c_str();
}

/// \return HashHexArray
char const * scytale_writer_get_fhashhex(ScytaleWriterHandle * handle) {
    SCOPED_TRACE;
    CHECK_HANDLE_R(handle, "");
    return handle->fhashhex.c_str();
}


ScytaleWriterHandle * scytale_writer_new(
    int with_encryption, int with_checksum, const char * master_derivator,
    get_hmac_key_prototype * hmac_fn, get_trace_key_prototype * trace_fn,
    int old_scheme, int one_shot)
{
    SCOPED_TRACE;
    return CREATE_HANDLE(ScytaleWriterHandle(
        ScytaleRandomWrapper::UDEV,
        with_encryption, with_checksum,
        hmac_fn, trace_fn,
        old_scheme, one_shot,
        master_derivator
    ));
}


ScytaleWriterHandle * scytale_writer_new_with_test_random(
    int with_encryption, int with_checksum, const char * master_derivator,
    get_hmac_key_prototype * hmac_fn, get_trace_key_prototype * trace_fn,
    int old_scheme, int one_shot)
{
    SCOPED_TRACE;
    return CREATE_HANDLE(ScytaleWriterHandle(
        ScytaleRandomWrapper::LCG,
        with_encryption, with_checksum,
        hmac_fn, trace_fn,
        old_scheme, one_shot,
        master_derivator
    ));
}

int scytale_writer_open(ScytaleWriterHandle * handle, const char * path, char const * hashpath, int groupid) {
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    handle->error_ctx.set_error(Error(NO_ERROR));
    CHECK_NOTHROW(handle->out_crypto_transport.open(path, hashpath, groupid/*, TODO derivator*/), ERR_TRANSPORT_OPEN_FAILED);
    return 0;
}

int scytale_writer_write(ScytaleWriterHandle * handle, uint8_t const * buffer, unsigned long len) {
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    CHECK_NOTHROW(handle->out_crypto_transport.send(buffer, len), ERR_TRANSPORT_WRITE_FAILED);
    return len;
}

int scytale_writer_close(ScytaleWriterHandle * handle) {
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    HashArray qhash;
    HashArray fhash;
    CHECK_NOTHROW(handle->out_crypto_transport.close(qhash, fhash), ERR_TRANSPORT_CLOSED);
    hash_to_hashhex(qhash, handle->qhashhex.data());
    hash_to_hashhex(fhash, handle->fhashhex.data());
    return 0;
}

void scytale_writer_delete(ScytaleWriterHandle * handle)
{
    SCOPED_TRACE;
    delete handle; /*NOLINT*/
}

char const * scytale_writer_get_error_message(ScytaleWriterHandle * handle)
{
    SCOPED_TRACE;
    return handle ? handle->error_ctx.message() : ScytaleErrorContext::handle_get_error_message();
}



ScytaleReaderHandle * scytale_reader_new(
    const char * master_derivator,
    get_hmac_key_prototype* hmac_fn, get_trace_key_prototype* trace_fn,
    int old_scheme, int one_shot)
{
    SCOPED_TRACE;
    return CREATE_HANDLE(ScytaleReaderHandle(
        InCryptoTransport::EncryptionMode::Auto,
        hmac_fn, trace_fn,
        old_scheme, one_shot,
        master_derivator
    ));
}

int scytale_reader_open(ScytaleReaderHandle * handle, char const * path, char const * derivator) {
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    handle->error_ctx.set_error(Error(NO_ERROR));
    CHECK_NOTHROW(
        handle->in_crypto_transport.open(path, {derivator, strlen(derivator)}),
        ERR_TRANSPORT_OPEN_FAILED);
    return 0;
}

int scytale_reader_open_with_auto_detect_encryption_scheme(
    ScytaleReaderHandle * handle, char const * path, char const * derivator)
{
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    handle->error_ctx.set_error(Error(NO_ERROR));
    bytes_view const derivator_array{derivator, strlen(derivator)};
    Error out_error{NO_ERROR};
    CHECK_NOTHROW(
        auto const r = open_if_possible_and_get_encryption_scheme_type(
            handle->in_crypto_transport, path, derivator_array, &out_error);
        switch (r)
        {
            case EncryptionSchemeTypeResult::Error:
                handle->error_ctx.set_error(out_error);
                break;
            case EncryptionSchemeTypeResult::OldScheme:
                // repopen file because some data are lost
                handle->cctxw.cctx.old_encryption_scheme = 1;
                handle->in_crypto_transport.open(path, derivator_array);
                break;
            case EncryptionSchemeTypeResult::NewScheme:
            case EncryptionSchemeTypeResult::NoEncrypted:
                break;
        }
        return int(r),
        ERR_TRANSPORT_OPEN_FAILED);
    return 0;
}


// 0: if end of file, len: if data was read, negative number on error
int scytale_reader_read(ScytaleReaderHandle * handle, uint8_t * buffer, unsigned long len) {
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    CHECK_NOTHROW(return int(handle->in_crypto_transport.partial_read(buffer, len)), ERR_TRANSPORT_READ_FAILED);
}


int scytale_reader_close(ScytaleReaderHandle * handle) {
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    CHECK_NOTHROW(handle->in_crypto_transport.close(), ERR_TRANSPORT_CLOSED);
    return 0;
}

void scytale_reader_delete(ScytaleReaderHandle * handle) {
    SCOPED_TRACE;
    delete handle; /*NOLINT*/
}

char const * scytale_reader_get_error_message(ScytaleReaderHandle * handle)
{
    return handle ? handle->error_ctx.message() : ScytaleErrorContext::handle_get_error_message();
}

int scytale_reader_fhash(ScytaleReaderHandle * handle, const char * file)
{
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    CHECK_NOTHROW(
        InCryptoTransport::HASH fhash = handle->in_crypto_transport.fhash(file);
        hash_to_hashhex(fhash.hash, handle->fhashhex.data()),
        ERR_TRANSPORT_READ_FAILED
    );
    return 0;
}

int scytale_reader_qhash(ScytaleReaderHandle * handle, const char * file)
{
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    CHECK_NOTHROW(
        InCryptoTransport::HASH qhash = handle->in_crypto_transport.qhash(file);
        hash_to_hashhex(qhash.hash, handle->qhashhex.data()),
        ERR_TRANSPORT_READ_FAILED
    );
    return 0;
}


const char * scytale_reader_get_qhashhex(ScytaleReaderHandle * handle)
{
    SCOPED_TRACE;
    CHECK_HANDLE_R(handle, nullptr);
    return handle->qhashhex.c_str();
}

const char * scytale_reader_get_fhashhex(ScytaleReaderHandle * handle)
{
    SCOPED_TRACE;
    CHECK_HANDLE_R(handle, nullptr);
    return handle->fhashhex.c_str();
}


struct ScytaleMetaReaderHandle
{
    explicit ScytaleMetaReaderHandle(ScytaleReaderHandle & reader)
      : mwrm_reader(reader.in_crypto_transport)
    {}

    ScytaleMwrmHeader & get_header() noexcept
    {
        auto const & header = this->mwrm_reader.get_header();
        this->c_header.version = static_cast<int>(header.version);
        this->c_header.has_checksum = header.has_checksum;
        return this->c_header;
    }

    ScytaleMwrmLine & get_meta_line() noexcept
    {
        if (this->meta_line.with_hash) {
            hash_to_hashhex(this->meta_line.hash1, this->hashhex1);
            hash_to_hashhex(this->meta_line.hash2, this->hashhex2);
        }
        else {
            this->hashhex1[0] = 0;
            this->hashhex2[0] = 0;
        }
        this->c_mwrm_line = {
            this->meta_line.filename,
            static_cast<uint64_t>(this->meta_line.size),
            this->meta_line.mode,
            this->meta_line.uid,
            this->meta_line.gid,
            this->meta_line.dev,
            this->meta_line.ino,
            static_cast<uint64_t>(this->meta_line.mtime),
            static_cast<uint64_t>(this->meta_line.ctime),
            static_cast<uint64_t>(this->meta_line.start_time),
            static_cast<uint64_t>(this->meta_line.stop_time),
            this->meta_line.with_hash,
            this->hashhex1,
            this->hashhex2,
        };
        return this->c_mwrm_line;
    }

private:
    HashHexArray hashhex1;
    HashHexArray hashhex2;

    ScytaleMwrmHeader c_header;
    ScytaleMwrmLine c_mwrm_line;

public:
    MwrmReader mwrm_reader;
    MetaLine meta_line;
    bool eof = false;

    ScytaleErrorContext error_ctx;
};


ScytaleMetaReaderHandle * scytale_meta_reader_new(ScytaleReaderHandle * reader)
{
    SCOPED_TRACE;
    CHECK_HANDLE_R(reader, nullptr);
    return CREATE_HANDLE(ScytaleMetaReaderHandle(*reader));
}

char const * scytale_meta_reader_get_error_message(ScytaleMetaReaderHandle * handle)
{
    SCOPED_TRACE;
    return handle ? handle->error_ctx.message() : ScytaleErrorContext::handle_get_error_message();
}

int scytale_meta_reader_read_hash(ScytaleMetaReaderHandle * handle, int version, int has_checksum)
{
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    handle->mwrm_reader.set_header({static_cast<WrmVersion>(version), bool(has_checksum)});
    CHECK_NOTHROW(
        handle->mwrm_reader.read_meta_hash_line(handle->meta_line),
        ERR_TRANSPORT_READ_FAILED);
    return 0;
}

int scytale_meta_reader_read_header(ScytaleMetaReaderHandle * handle)
{
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    CHECK_NOTHROW(
        handle->mwrm_reader.read_meta_headers(),
        ERR_TRANSPORT_READ_FAILED);
    return 0;
}

int scytale_meta_reader_read_line(ScytaleMetaReaderHandle * handle)
{
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    CHECK_NOTHROW(
        handle->eof
            = Transport::Read::Eof == handle->mwrm_reader.read_meta_line(handle->meta_line),
        ERR_TRANSPORT_READ_FAILED);
    return handle->eof ? ERR_TRANSPORT_NO_MORE_DATA : 0;
}

int scytale_meta_reader_read_line_eof(ScytaleMetaReaderHandle * handle)
{
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    return handle->eof;
}

void scytale_meta_reader_delete(ScytaleMetaReaderHandle * handle)
{
    SCOPED_TRACE;
    delete handle; /*NOLINT*/
}

ScytaleMwrmHeader * scytale_meta_reader_get_header(ScytaleMetaReaderHandle * handle)
{
    SCOPED_TRACE;
    CHECK_HANDLE_R(handle, nullptr);
    return &handle->get_header();
}

ScytaleMwrmLine * scytale_meta_reader_get_line(ScytaleMetaReaderHandle * handle)
{
    SCOPED_TRACE;
    CHECK_HANDLE_R(handle, nullptr);
    return &handle->get_meta_line();
}


struct ScytaleKeyHandle
{
    HashHexArray masterhex;
    HashArray master;
    HashHexArray derivatedhex;
    HashArray derivated;

    explicit ScytaleKeyHandle(const char * masterkeyhex)
    {
        memcpy(this->masterhex, masterkeyhex, sizeof(this->masterhex));
        hashex_to_hash(this->masterhex, this->master);
    }
};

ScytaleKeyHandle * scytale_key_new(const char * masterkeyhex)
{
    SCOPED_TRACE;
    constexpr auto key_len = sizeof(HashHexArray) - 1;
    if (!masterkeyhex || strlen(masterkeyhex) != key_len){
        return nullptr;
    }
    for (char c : make_array_view(masterkeyhex, key_len)){
        if (not ((c >= '0' and c <= '9') or (c >= 'A' and c <= 'F') or (c >= 'a' and c <= 'f'))){
            return nullptr;
        }
    }
    return CREATE_HANDLE(ScytaleKeyHandle(masterkeyhex));
}

const char * scytale_key_derivate(ScytaleKeyHandle * handle, const uint8_t * derivator, unsigned long len)
{
    SCOPED_TRACE;
    CHECK_HANDLE_R(handle, "");

    std::unique_ptr<uint8_t[]> normalized_derivator_gc;
    auto const new_derivator = CryptoContext::get_normalized_derivator(
        normalized_derivator_gc, {derivator, len});

    CryptoContext cctx;
    cctx.old_encryption_scheme = false;
    cctx.one_shot_encryption_scheme = false;
    cctx.set_master_key(handle->master);
    cctx.get_derived_key(handle->derivated, new_derivator);
    hash_to_hashhex(handle->derivated, handle->derivatedhex);
    return handle->derivatedhex;
}

void scytale_key_delete(ScytaleKeyHandle * handle) {
    SCOPED_TRACE;
    delete handle; /*NOLINT*/
}

const char * scytale_key_get_master(ScytaleKeyHandle * handle) {
    SCOPED_TRACE;
    CHECK_HANDLE_R(handle, "");
    return handle->masterhex;
}

const char * scytale_key_get_derivated(ScytaleKeyHandle * handle) {
    SCOPED_TRACE;
    CHECK_HANDLE_R(handle, "");
    return handle->derivatedhex;
}


struct ScytaleTflWriterHandler
{
    ScytaleTflWriterHandler(
        ScytaleFdxWriterHandle& fdx,
        uint64_t idx, std::string&& original_filename,
        std::string finalname, char const* hash_filename, std::size_t pos_filename,
        int groupid, CryptoContext& cctx, Random& random, Fstat& fstat)
    : idx(idx)
    , finalname(std::move(finalname))
    , pos_filename(pos_filename)
    , original_filename(std::move(original_filename))
    , out_crypto_transport(cctx, random, fstat)
    , fdx(fdx)
    {
        auto derivator = array_view(this->finalname).drop_front(pos_filename);
        this->out_crypto_transport.open(
            this->finalname.c_str(), hash_filename, groupid, derivator);
    }

    uint64_t idx;
    std::string finalname;
    std::size_t pos_filename;
    std::string original_filename;

    OutCryptoTransport out_crypto_transport;

    ScytaleFdxWriterHandle& fdx;
};

struct ScytaleFdxWriterHandle
{
    static constexpr array_view_const_char fdx_suffix = ".fdx"_av;

    ScytaleFdxWriterHandle(
        int with_encryption, int with_checksum, const char * master_derivator,
        get_hmac_key_prototype * hmac_fn, get_trace_key_prototype * trace_fn,
        ScytaleRandomWrapper::RandomType random_type)
    : random_wrapper(random_type)
    , cctxw(hmac_fn, trace_fn, with_encryption, with_checksum, false, false, master_derivator)
    , out_crypto_transport(cctxw.cctx, *random_wrapper.rnd, fstat)
    {
        this->qhashhex[0] = 0;
        this->fhashhex[0] = 0;
    }

    void open(std::string_view path, std::string_view hashpath, int groupid, std::string_view sid)
    {
        str_append(this->prefix, path, '/', sid, fdx_suffix);
        str_append(this->hash_prefix, hashpath, '/', sid, fdx_suffix);
        this->pos_filename = this->prefix.size() - sid.size() - fdx_suffix.size();
        this->groupid = groupid;

        auto derivator = array_view(this->prefix).drop_front(this->pos_filename);
        this->out_crypto_transport.open(
            this->prefix.c_str(), this->hash_prefix.c_str(), groupid, derivator);
        this->out_crypto_transport.send(Mwrm3::top_header);
        this->prefix.erase(this->prefix.size() - fdx_suffix.size());
        this->prefix += ',';
        this->hash_prefix.erase(this->hash_prefix.size() - fdx_suffix.size());
        this->hash_prefix += ',';
    }

    ScytaleTflWriterHandler * open_tfl(char const* filename)
    {
        ++this->idx;

        std::array<char, std::numeric_limits<uint64_t>::digits10+2> buf;
        char* const p = begin(buf);
        std::to_chars_result const chars_result = std::to_chars(p, end(buf), this->idx);
        std::ptrdiff_t const len = std::distance(p, chars_result.ptr);

        array_view const leftnum = "000000"_av.first(len < 6 ? 6 - len : 0);
        array_view const rightnum = std::string_view(p, len);
        array_view const ext = ".tfl"_av;

        auto const old_size = this->prefix.size();
        auto const old_hash_size = this->hash_prefix.size();

        str_append(this->prefix, leftnum, rightnum, ext);
        str_append(this->hash_prefix, array_view(this->prefix).drop_front(old_size));

        auto* tfl = new(std::nothrow) ScytaleTflWriterHandler{ /*NOLINT*/
            *this,
            // truncate filename if too long
            this->idx, std::string(filename, strnlen(filename, ~uint16_t())),
            this->prefix, this->hash_prefix.c_str(), this->pos_filename,
            this->groupid, this->cctxw.cctx, *this->random_wrapper.rnd, this->fstat,
        };

        this->prefix.erase(old_size);
        this->hash_prefix.erase(old_hash_size);

        return tfl;
    }

    int close_tfl(ScytaleTflWriterHandler& tfl)
    {
        HashArray qhash;
        HashArray fhash;
        tfl.out_crypto_transport.close(qhash, fhash);

        constexpr unsigned buf_size = 4096;
        uint8_t cbuf[buf_size];
        unsigned buf_pos = 0;

        auto write_data = [&](bytes_view bytes){
            if (buf_pos + bytes.size() < buf_size)
            {
                memcpy(cbuf + buf_pos, bytes.data(), bytes.size());
                buf_pos += bytes.size();
            }
            else
            {
                memcpy(cbuf + buf_pos, bytes.data(), buf_size - buf_pos);
                this->out_crypto_transport.send(cbuf, buf_size);

                auto* p = bytes.data() + buf_size - buf_pos;
                auto* end = bytes.end();

                for (;;)
                {
                    if (p + buf_size > end)
                    {
                        buf_pos = end - p;
                        memcpy(cbuf, p, buf_pos);
                        break;
                    }

                    this->out_crypto_transport.send(p, buf_size);
                    p += buf_size;
                }
            }
        };

        auto write_in_buf = [&](Mwrm3::Type /*type*/, auto... data) {
            (write_data(data), ...);
        };

        auto const fsize = [&]{
            int64_t signed_fsize { filesize(tfl.finalname.c_str()) };
            if (REDEMPTION_UNLIKELY(signed_fsize < 0)) {
                this->error_ctx.set_error(Error(ERR_TRANSPORT, errno));
                return ~uint64_t{};
            }
            return uint64_t(signed_fsize);
        }();

        Mwrm3::serialize_tfl_new(
            tfl.idx, tfl.original_filename,
            array_view(tfl.finalname).drop_front(tfl.pos_filename),
            write_in_buf);
        Mwrm3::serialize_tfl_stat(tfl.idx, safe_cast<Mwrm3::FileSize>(fsize),
            Mwrm3::QuickHash{this->with_checksum() ? make_array_view(qhash) : bytes_view{}},
            Mwrm3::FullHash{this->with_checksum() ? make_array_view(fhash) : bytes_view{}},
            write_in_buf);

        if (buf_pos)
        {
            this->out_crypto_transport.send(cbuf, buf_pos);
        }

        return fsize != ~uint64_t{} ? 0 : -1;
    }

    int cancel_tfl(ScytaleTflWriterHandler& tfl)
    {
        return tfl.out_crypto_transport.cancel() ? 0 : -1;
    }

    void close()
    {
        HashArray qhash;
        HashArray fhash;
        this->out_crypto_transport.close(qhash, fhash);
        hash_to_hashhex(qhash, this->qhashhex);
        hash_to_hashhex(fhash, this->fhashhex);

        this->prefix.clear();
        this->hash_prefix.clear();
        this->pos_filename = 0;
    }

private:
    bool with_checksum() const noexcept
    {
        return this->cctxw.cctx.get_with_checksum();
    }

    uint64_t idx = 0;
    std::string prefix;
    std::string hash_prefix;
    std::size_t pos_filename = 0;
    int groupid = 0;
    Fstat fstat;
    ScytaleRandomWrapper random_wrapper;

    CryptoContextWrapper cctxw;

    OutCryptoTransport out_crypto_transport;

public:
    ScytaleErrorContext error_ctx;
    HashHexArray qhashhex;
    HashHexArray fhashhex;
};


ScytaleFdxWriterHandle * scytale_fdx_writer_new(
    int with_encryption, int with_checksum, char const* master_derivator,
    get_hmac_key_prototype * hmac_fn, get_trace_key_prototype * trace_fn)
{
    SCOPED_TRACE;
    return CREATE_HANDLE(ScytaleFdxWriterHandle(
        with_encryption, with_checksum, master_derivator,
        hmac_fn, trace_fn, ScytaleRandomWrapper::UDEV));
}

ScytaleFdxWriterHandle * scytale_fdx_writer_new_with_test_random(
    int with_encryption, int with_checksum, char const* master_derivator,
    get_hmac_key_prototype * hmac_fn, get_trace_key_prototype * trace_fn)
{
    SCOPED_TRACE;
    return CREATE_HANDLE(ScytaleFdxWriterHandle(
        with_encryption, with_checksum, master_derivator,
        hmac_fn, trace_fn, ScytaleRandomWrapper::LCG));
}

int scytale_fdx_writer_open(
    ScytaleFdxWriterHandle * handle,
    char const * path, char const * hashpath, int groupid, char const * sid)
{
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    CHECK_NOTHROW(handle->open(path, hashpath, groupid, sid), ERR_TRANSPORT_OPEN_FAILED);
    return 0;
}

ScytaleTflWriterHandler * scytale_fdx_writer_open_tfl(
    ScytaleFdxWriterHandle * handle, char const * filename)
{
    SCOPED_TRACE;
    CHECK_HANDLE_R(handle, nullptr);
    CHECK_NOTHROW_R(return handle->open_tfl(filename),
        nullptr, handle->error_ctx, ERR_TRANSPORT_OPEN_FAILED);
}

int scytale_tfl_writer_write(
    ScytaleTflWriterHandler * handle, uint8_t const * buffer, unsigned long len)
{
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    CHECK_NOTHROW_R(handle->out_crypto_transport.send(buffer, len),
        -1, handle->fdx.error_ctx, ERR_TRANSPORT_WRITE_FAILED);
    return len;
}

int scytale_tfl_writer_close(ScytaleTflWriterHandler * handle)
{
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    std::unique_ptr<ScytaleTflWriterHandler> auto_delete{handle};
    CHECK_NOTHROW_R(return handle->fdx.close_tfl(*handle),
        -1, handle->fdx.error_ctx, ERR_TRANSPORT_WRITE_FAILED);
}

int scytale_tfl_writer_cancel(ScytaleTflWriterHandler * handle)
{
    SCOPED_TRACE;
    CHECK_HANDLE(handle);
    std::unique_ptr<ScytaleTflWriterHandler> auto_delete{handle};
    return handle->fdx.cancel_tfl(*handle);
}

/// \return HashHexArray
char const * scytale_fdx_writer_get_qhashhex(ScytaleFdxWriterHandle * handle) {
    SCOPED_TRACE;
    return handle ? handle->qhashhex : "";
}

/// \return HashHexArray
char const * scytale_fdx_writer_get_fhashhex(ScytaleFdxWriterHandle * handle) {
    SCOPED_TRACE;
    return handle ? handle->fhashhex : "";
}

int scytale_fdx_writer_close(ScytaleFdxWriterHandle * handle)
{
    SCOPED_TRACE;
    CHECK_NOTHROW(handle->close(), ERR_TRANSPORT_CLOSED);
    return 0;
}

int scytale_fdx_writer_delete(ScytaleFdxWriterHandle * handle)
{
    SCOPED_TRACE;
    delete handle; /*NOLINT*/
    return 0;
}

char const * scytale_fdx_writer_get_error_message(ScytaleFdxWriterHandle * handle)
{
    SCOPED_TRACE;
    return handle ? handle->error_ctx.message() : ScytaleErrorContext::handle_get_error_message();
}

}
