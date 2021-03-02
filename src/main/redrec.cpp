#include "lib/do_recorder.hpp"
#include "utils/chex_to_int.hpp"

#include <syslog.h>
#include <iostream>
#include <vector>

// ./bin/gcc-5.4.0/release/redrec redver -i ./tests/includes/fixtures/verifier/recorded/toto@10.10.43.13,Administrateur@QA@cible,20160218-183009,wab-5-0-0.yourdomain,7335.mwrm -m ./tests/includes/fixtures/verifier/recorded/ -s ./tests/fixtures/verifier/hash/ --verbose 10

namespace
{
    uint8_t g_key[CRYPTO_KEY_LENGTH] {};

    int get_trace_key(uint8_t const * /*base*/, int /*len*/, uint8_t * buffer, unsigned /*oldscheme*/)
    {
        memcpy(buffer, g_key, sizeof(g_key));
        return 0;
    }

    void usage()
    {
        std::cerr << "Usage [{rec|ver|dec}] [hex-hmac_key hex-key] lib-args..." << std::endl;
    }
}


int main(int argc, const char** argv)
{
    int arg_used = 0;
    char const * command = "redrec";
    if (argc > 1)
    {
        struct P { char const * shortname, * name; };
        for (P pair : {P{"rec", "redrec"}, P{"ver", "redver"}, P{"dec", "reddec"}})
        {
            if (0 == strcmp(argv[1], pair.shortname) || 0 == strcmp(argv[1], pair.name))
            {
                command = pair.name;
                ++arg_used;
                break;
            }
        }
    }

    if (argc < 2+arg_used || !(argv[arg_used+1][0] == '-' || (argc > arg_used+3 && argv[arg_used+3][0] == '-')))
    {
        usage();
        return 1;
    }

    std::vector<char const*> new_argv;
    uint8_t hmac_key_buffer[CRYPTO_KEY_LENGTH] {};
    bool has_hmac_key = false;

    if (argv[arg_used+1][0] == '-')
    {
        auto set_default_key = [](uint8_t (&key)[CRYPTO_KEY_LENGTH])
        {
            for (std::size_t i = 0; i < CRYPTO_KEY_LENGTH; ++i)
            {
                key[i] = uint8_t(i);
            }
        };
        set_default_key(hmac_key_buffer);
        set_default_key(g_key);

        if (arg_used)
        {
            argv[arg_used] = command;
        }
        else
        {
            // create new argc, argv
            new_argv.resize(argc+2u);
            new_argv.back() = nullptr;
            new_argv[0] = argv[0];
            new_argv[1] = command;
            for (int i = 1; i < argc; ++i)
            {
                new_argv[i+1] = argv[i];
            }
            ++argc;
            argv = new_argv.data();
        }
    }
    else
    {
        if (strlen(argv[arg_used+1]) != 64 || strlen(argv[arg_used+2]) != 64)
        {
            std::cerr << argv[0] << ": hmac_key or key len is not 64\n";
            return 1;
        }

        auto load_key = [](uint8_t (&key)[CRYPTO_KEY_LENGTH], char const * hexkey)
        {
            int err = 0;
            for (std::size_t i = 0; i < CRYPTO_KEY_LENGTH; ++i)
            {
                key[i]
                = (chex_to_int(hexkey[i*2], err) << 4)
                | chex_to_int(hexkey[i*2+1], err);
            }
            return !err;
        };

        if (!load_key(hmac_key_buffer, argv[arg_used+1]) || !load_key(g_key, argv[arg_used+2]))
        {
            std::cerr << argv[0] << ": invalid format to hmac_key or key\n";
            return 1;
        }

        has_hmac_key = true;

        argc -= arg_used + 1;
        arg_used += 2;
        argv[arg_used-1] = argv[0];
        argv[arg_used] = command;
        argv = argv + arg_used - 1;
    }

    openlog("redrec", LOG_PERROR, LOG_USER);
    return do_main(argc, argv, has_hmac_key ? hmac_key_buffer : nullptr, get_trace_key);
}
