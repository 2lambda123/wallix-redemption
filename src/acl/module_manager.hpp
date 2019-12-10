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
  Copyright (C) Wallix 2010
  Author(s): Christophe Grosjean, Javier Caverni, Xavier Dunat,
             Raphael Zhou, Meng Tan

  Manage Modules Life cycle : creation, destruction and chaining
  find out the next module to run from context reading
*/

#pragma once

#include "acl/auth_api.hpp"
#include "acl/file_system_license_store.hpp"
#include "acl/mm_api.hpp"
#include "acl/module_manager/mm_ini.hpp"
#include "acl/module_manager/enums.hpp"
#include "configs/config.hpp"
#include "core/log_id.hpp"
#include "core/session_reactor.hpp"
#include "front/front.hpp"
#include "gdi/protected_graphics.hpp"

#include "mod/internal/bouncer2_mod.hpp"
#include "RAIL/client_execute.hpp"
#include "mod/internal/flat_dialog_mod.hpp"
#include "mod/internal/flat_login_mod.hpp"
#include "mod/internal/flat_wab_close_mod.hpp"
#include "mod/internal/flat_wait_mod.hpp"
#include "mod/internal/interactive_target_mod.hpp"
#include "mod/internal/rail_module_host_mod.hpp"
#include "mod/internal/replay_mod.hpp"
#include "mod/internal/selector_mod.hpp"
#include "mod/internal/test_card_mod.hpp"
#include "mod/internal/widget_test_mod.hpp"
#include "mod/internal/transition_mod.hpp"

#include "mod/mod_api.hpp"
#include "mod/null/null.hpp"
#include "mod/rdp/windowing_api.hpp"
#include "mod/xup/xup.hpp"

#include "transport/socket_transport.hpp"

#include "utils/load_theme.hpp"
#include "utils/netutils.hpp"
#include "utils/sugar/algostring.hpp"
#include "utils/sugar/scope_exit.hpp"
#include "utils/sugar/update_lock.hpp"
#include "utils/translation.hpp"
#include "utils/log_siem.hpp"
#include "utils/fileutils.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

inline void add_time_before_closing(std::string & msg, uint32_t elapsed_time, Translator tr)
{
    const auto hours = elapsed_time / (60*60);
    const auto minutes = elapsed_time / 60 - hours * 60;
    const auto seconds = elapsed_time - hours * (60*60) - minutes * 60;

    if (hours) {
        str_append(
            msg,
            std::to_string(hours),
            ' ',
            tr(trkeys::hour),
            (hours > 1) ? "s, " : ", "
        );
    }

    if (minutes || hours) {
        str_append(
            msg,
            std::to_string(minutes),
            ' ',
            tr(trkeys::minute),
            (minutes > 1) ? "s, " : ", "
        );
    }

    str_append(
        msg,
        std::to_string(seconds),
        ' ',
        tr(trkeys::second),
        (seconds > 1) ? "s " : " ",
        tr(trkeys::before_closing)
    );
}

class EndSessionWarning
{
    static constexpr std::array<unsigned, 4> timers{{ 30*60, 10*60, 5*60, 1*60 }};

    const unsigned OSD_STATE_INVALID = timers.size();
    const unsigned OSD_STATE_NOT_YET_COMPUTED = OSD_STATE_INVALID + 1;

    unsigned osd_state = OSD_STATE_NOT_YET_COMPUTED;

public:
    void initialize() {
        this->osd_state = OSD_STATE_NOT_YET_COMPUTED;
    }

    void update_osd_state(std::string& out_msg, Translation::language_t lang, time_t start_time, time_t end_time, time_t now) {
        out_msg.clear();

        if (this->osd_state == OSD_STATE_NOT_YET_COMPUTED) {
            this->osd_state = (
                      (end_time <= now)
                    ? OSD_STATE_INVALID
                    : timers.size() - (std::lower_bound(timers.rbegin(), timers.rend(), end_time - start_time) - timers.rbegin())
                );
        }
        else if (this->osd_state < OSD_STATE_INVALID
              && end_time - now <= timers[this->osd_state]) {
            out_msg.reserve(128);
            const unsigned minutes = (end_time - now + 30) / 60;
            out_msg += std::to_string(minutes);
            out_msg += ' ';
            out_msg += TR(trkeys::minute, lang);
            out_msg += (minutes > 1) ? "s " : " ";
            out_msg += TR(trkeys::before_closing, lang);

            ++this->osd_state;
        }
    }
};

class CryptoContext;

class ModuleManager : public MMIni
{
    class ModOSD : public gdi::ProtectedGraphics, public mod_api
    {
        ModuleManager & mm;

        std::string osd_message;
        Rect clip;
        RDPColor color;
        RDPColor background_color;
        bool is_disable_by_input = false;
        bool bogus_refresh_rect_ex;

    public:
        explicit ModOSD(ModuleManager & mm)
        : gdi::ProtectedGraphics(mm.front, Rect{})
        , mm(mm)
        , bogus_refresh_rect_ex(false)
        {}

        [[nodiscard]] bool is_input_owner() const { return this->is_disable_by_input; }

        void disable_osd()
        {
            this->is_disable_by_input = false;
            auto const protected_rect = this->get_protected_rect();
            this->set_protected_rect(Rect{});

            if (this->bogus_refresh_rect_ex) {
                this->mm.mod->rdp_suppress_display_updates();
                this->mm.mod->rdp_allow_display_updates(0, 0,
                    this->mm.front.client_info.screen_info.width,
                    this->mm.front.client_info.screen_info.height);
            }

            if (this->mm.winapi) {
                this->mm.winapi->destroy_auxiliary_window();
            }

            this->mm.mod->rdp_input_invalidate(protected_rect);
        }

        [[nodiscard]] const char* get_message() const {
            return this->osd_message.c_str();
        }

        void set_message(std::string message, bool is_disable_by_input)
        {
            this->osd_message = std::move(message);
            this->is_disable_by_input = is_disable_by_input;
            this->bogus_refresh_rect_ex = (this->mm.ini.get<cfg::globals::bogus_refresh_rect>()
             && this->mm.ini.get<cfg::globals::allow_using_multiple_monitors>()
             && (this->mm.front.client_info.cs_monitor.monitorCount > 1));

            if (is_disable_by_input) {
                str_append(this->osd_message, "  ", TR(trkeys::disable_osd, language(this->mm.ini)));
            }

            gdi::TextMetrics tm(this->mm.load_font(), this->osd_message.c_str());
            int w = tm.width + padw * 2;
            int h = tm.height + padh * 2;
            this->color = color_encode(BGRColor(BLACK), this->mm.front.client_info.screen_info.bpp);
            this->background_color = color_encode(BGRColor(LIGHT_YELLOW), this->mm.front.client_info.screen_info.bpp);

            if (this->mm.front.client_info.remote_program &&
                (this->mm.winapi == static_cast<windowing_api*>(&this->mm.rail_client_execute))) {

                Rect current_work_area_rect = this->mm.rail_client_execute.get_current_work_area_rect();

                this->clip = Rect(
                    current_work_area_rect.x +
                        (current_work_area_rect.cx < w ? 0 : (current_work_area_rect.cx - w) / 2),
                    0, w, h);
            }
            else {
                this->clip = Rect(this->mm.front.client_info.screen_info.width < w ? 0 : (this->mm.front.client_info.screen_info.width - w) / 2, 0, w, h);
            }

            this->set_protected_rect(this->clip);

            if (this->mm.winapi) {
                this->mm.winapi->create_auxiliary_window(this->clip);
            }
        }

        static constexpr int padw = 16;
        static constexpr int padh = 16;

        void draw_osd_message()
        {
            this->mm.front.begin_update();
            this->draw_osd_message_impl(this->mm.front);
            this->mm.front.end_update();
        }

        bool try_input_scancode(long param1, long param2, long param3, long param4, Keymap2 * keymap)
        {
            (void)param1;
            (void)param2;
            (void)param3;
            (void)param4;
            if (this->is_disable_by_input
             && keymap->nb_kevent_available() > 0
//             && !(param3 & SlowPath::KBDFLAGS_DOWN)
             && keymap->top_kevent() == Keymap2::KEVENT_INSERT
            ) {
                keymap->get_kevent();
                this->disable_osd();
                return true;
            }
            return false;
        }

        bool try_input_mouse(int device_flags, int x, int y, Keymap2 * /*unused*/)
        {
            if (this->is_disable_by_input
             && this->get_protected_rect().contains_pt(x, y)
             && device_flags == (MOUSE_FLAG_BUTTON1|MOUSE_FLAG_DOWN)) {
                this->disable_osd();
                return true;
            }
            return false;
        }

        bool try_input_invalidate(const Rect r)
        {
            if (!this->get_protected_rect().isempty() && r.has_intersection(this->get_protected_rect())) {
                auto rects = gdi::subrect4(r, this->get_protected_rect());
                auto p = std::begin(rects);
                auto e = std::remove_if(p, std::end(rects), [](Rect const & rect) {
                    return rect.isempty();
                });
                if (p != e) {
                    this->mm.mod->rdp_input_invalidate2({p, e});
                    this->clip = r.intersect(this->get_protected_rect());
                }
                return true;
            }
            return false;
        }

        bool try_input_invalidate2(array_view<Rect const> vr)
        {
            // TODO PERF multi opaque rect
            bool ret = false;
            for (Rect const & r : vr) {
                if (!this->try_input_invalidate(r)) {
                    this->mm.mod->rdp_input_invalidate(r);
                }
                else {
                    ret = true;
                }
            }
            return ret;
        }

    private:
        void draw_osd_message_impl(gdi::GraphicApi & drawable)
        {
            if (this->clip.isempty()) {
                return ;
            }

            auto const color_ctx = gdi::ColorCtx::from_bpp(this->mm.front.client_info.screen_info.bpp, this->mm.front.get_palette());

            drawable.draw(RDPOpaqueRect(this->clip, this->background_color), this->clip, color_ctx);

            RDPLineTo line_ileft(1, this->clip.x, this->clip.y, this->clip.x, this->clip.y + this->clip.cy - 1,
                encode_color24()(BLACK), 0x0D, RDPPen(0, 0, encode_color24()(BLACK)));
            drawable.draw(line_ileft, this->clip, color_ctx);
            RDPLineTo line_ibottom(1, this->clip.x, this->clip.y + this->clip.cy - 1, this->clip.x + this->clip.cx - 1, this->clip.y + this->clip.cy - 1,
                encode_color24()(BLACK), 0x0D, RDPPen(0, 0, encode_color24()(BLACK)));
            drawable.draw(line_ibottom, this->clip, color_ctx);

            RDPLineTo line_iright(1, this->clip.x + this->clip.cx - 1, this->clip.y + this->clip.cy - 1, this->clip.x + this->clip.cx - 1, this->clip.y,
                encode_color24()(BLACK), 0x0D, RDPPen(0, 0, encode_color24()(BLACK)));
            drawable.draw(line_iright, this->clip, color_ctx);
            RDPLineTo line_etop(1, this->clip.x + this->clip.cx - 1, this->clip.y, this->clip.x, this->clip.y,
                encode_color24()(BLACK), 0x0D, RDPPen(0, 0, encode_color24()(BLACK)));
            drawable.draw(line_etop, this->clip, color_ctx);

            gdi::server_draw_text(
                drawable, this->mm.load_font(),
                this->get_protected_rect().x + padw, padh,
                this->osd_message.c_str(),
                this->color, this->background_color, color_ctx, this->clip
            );

            this->clip = Rect();
        }

        void refresh_rects(array_view<Rect const> av) override
        {
            this->mm.mod->rdp_input_invalidate2(av);
        }

        void rdp_input_scancode(long param1, long param2, long param3, long param4, Keymap2 * keymap) override
        {
            if (!this->try_input_scancode(param1, param2, param3, param4, keymap)) {
                this->mm.mod->rdp_input_scancode(param1, param2, param3, param4, keymap);
            }
        }

        void rdp_input_unicode(uint16_t unicode, uint16_t flag) override {
            this->mm.mod->rdp_input_unicode(unicode, flag);
        }

        void rdp_input_mouse(int device_flags, int x, int y, Keymap2 * keymap) override
        {
            if (!this->try_input_mouse(device_flags, x, y, keymap)) {
                this->mm.mod->rdp_input_mouse(device_flags, x, y, keymap);
            }
        }

        void rdp_input_invalidate(Rect r) override
        {
            if (!this->try_input_invalidate(r)) {
                this->mm.mod->rdp_input_invalidate(r);
            }
        }

        void rdp_input_invalidate2(array_view<Rect const> vr) override
        {
            if (!this->try_input_invalidate2(vr)) {
                this->mm.mod->rdp_input_invalidate2(vr);
            }
        }

        void rdp_input_synchronize(uint32_t time, uint16_t device_flags, int16_t param1, int16_t param2) override
        { this->mm.mod->rdp_input_synchronize(time, device_flags, param1, param2); }

        void rdp_input_up_and_running() override
        { this->mm.mod->rdp_input_up_and_running(); }

        void rdp_allow_display_updates(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom) override
        { this->mm.mod->rdp_allow_display_updates(left, top, right, bottom); }

        void rdp_suppress_display_updates() override
        { this->mm.mod->rdp_suppress_display_updates(); }

        void refresh(Rect r) override
        {
            this->mm.mod->refresh(r);
        }

        void send_to_mod_channel(
            CHANNELS::ChannelNameId front_channel_name, InStream & chunk,
            std::size_t length, uint32_t flags
        ) override
        { this->mm.mod->send_to_mod_channel(front_channel_name, chunk, length, flags); }

        void send_auth_channel_data(const char * data) override
        { this->mm.mod->send_auth_channel_data(data); }

        void send_checkout_channel_data(const char * data) override
        { this->mm.mod->send_checkout_channel_data(data); }

        [[nodiscard]] bool is_up_and_running() const override
        { return this->mm.mod->is_up_and_running(); }

        void disconnect() override
        { this->mm.mod->disconnect(); }

        void display_osd_message(std::string const & message) override
        { this->mm.mod->display_osd_message(message); }

        [[nodiscard]] Dimension get_dim() const override
        {
            return this->mm.mod->get_dim();
        }
    };

    FileSystemLicenseStore file_system_license_store{ app_path(AppPath::License).to_string() };

    class sock_mod_barrier {};

    template<class Mod>
    class ModWithSocket final : private SocketTransport, public Mod
    {
        ModuleManager & mm;
        bool target_info_is_shown = false;

    public:
        template<class... Args>
        ModWithSocket(
            ModuleManager & mm, AuthApi & /*authentifier*/,
            const char * name, unique_fd sck, uint32_t verbose,
            std::string * error_message, sock_mod_barrier /*unused*/, Args && ... mod_args)
        : SocketTransport( name, std::move(sck)
                         , mm.ini.get<cfg::context::target_host>().c_str()
                         , mm.ini.get<cfg::context::target_port>()
                         , std::chrono::milliseconds(mm.ini.get<cfg::globals::mod_recv_timeout>())
                         , to_verbose_flags(verbose), error_message)
        , Mod(*this, std::forward<Args>(mod_args)...)
        , mm(mm)
        {
            this->mm.socket_transport = this;
        }

        ~ModWithSocket()
        {
            this->mm.socket_transport = nullptr;
            log_proxy::target_disconnection(
                this->mm.ini.template get<cfg::context::auth_error_message>().c_str());
        }

        void display_osd_message(std::string const & message) override {
            this->mm.osd_message(message, true);
        }

        void rdp_input_scancode(long param1, long param2, long param3, long param4, Keymap2 * keymap) override
        {
            //LOG(LOG_INFO, "mod_osd::rdp_input_scancode: keyCode=0x%X keyboardFlags=0x%04X this=<%p>", param1, param3, this);
            if (this->mm.mod_osd.try_input_scancode(param1, param2, param3, param4, keymap)) {
                this->target_info_is_shown = false;
                return ;
            }

            Mod::rdp_input_scancode(param1, param2, param3, param4, keymap);

            if (this->mm.ini.template get<cfg::globals::enable_osd_display_remote_target>() && (param1 == Keymap2::F12)) {
                bool const f12_released = (param3 & SlowPath::KBDFLAGS_RELEASE);
                if (this->target_info_is_shown && f12_released) {
                    // LOG(LOG_INFO, "Hide info");
                    this->mm.clear_osd_message();
                    this->target_info_is_shown = false;
                }
                else if (!this->target_info_is_shown && !f12_released) {
                    // LOG(LOG_INFO, "Show info");
                    std::string msg;
                    msg.reserve(64);
                    if (this->mm.ini.template get<cfg::client::show_target_user_in_f12_message>()) {
                        msg  = this->mm.ini.template get<cfg::globals::target_user>();
                        msg += "@";
                    }
                    msg += this->mm.ini.template get<cfg::globals::target_device>();
                    const uint32_t enddate = this->mm.ini.template get<cfg::context::end_date_cnx>();
                    if (enddate) {
                        const auto now = time(nullptr);
                        const auto elapsed_time = enddate - now;
                        // only if "reasonable" time
                        if (elapsed_time < 60*60*24*366L) {
                            msg += "  [";
                            add_time_before_closing(msg, elapsed_time, Translator(this->mm.ini));
                            msg += ']';
                        }
                    }
                    this->mm.osd_message(std::move(msg), false);
                    this->target_info_is_shown = true;
                }
            }
        }

        void rdp_input_mouse(int device_flags, int x, int y, Keymap2 * keymap) override
        {
            if (this->mm.mod_osd.try_input_mouse(device_flags, x, y, keymap)) {
                this->target_info_is_shown = false;
                return ;
            }

            Mod::rdp_input_mouse(device_flags, x, y, keymap);
        }

        void rdp_input_unicode(uint16_t unicode, uint16_t flag) override {
            Mod::rdp_input_unicode(unicode, flag);
        }

        void rdp_input_invalidate(const Rect r) override
        {
            if (this->mm.mod_osd.try_input_invalidate(r)) {
                return ;
            }

            Mod::rdp_input_invalidate(r);
        }

        void rdp_input_invalidate2(array_view<Rect const> vr) override
        {
            if (this->mm.mod_osd.try_input_invalidate2(vr)) {
                return ;
            }

            Mod::rdp_input_invalidate2(vr);
        }
    };


public:
    void DLP_antivirus_check_channels_files() {
        this->mod->DLP_antivirus_check_channels_files();
    }

    gdi::GraphicApi & get_graphic_wrapper()
    {
        gdi::GraphicApi& gd = this->mod_osd.get_protected_rect().isempty()
          ? static_cast<gdi::GraphicApi&>(this->front) : this->mod_osd;
        if (this->rail_module_host_mod_ptr) {
            return this->rail_module_host_mod_ptr->proxy_gd(gd);
        }
        return gd;
    }

    Callback & get_callback() noexcept
    {
        return *this->mod;
    }

    void clear_osd_message()
    {
        if (!this->mod_osd.get_protected_rect().isempty()) {
            this->mod_osd.disable_osd();
        }
    }

    void osd_message(std::string message, bool is_disable_by_input)
    {
        if (message != this->mod_osd.get_message()) {
            this->clear_osd_message();
        }
        if (!message.empty()) {
            this->mod_osd.set_message(std::move(message), is_disable_by_input);
            this->mod_osd.draw_osd_message();
        }
    }

private:
    RailModuleHostMod* rail_module_host_mod_ptr = nullptr;
    Front & front;
    null_mod no_mod;
    ModOSD mod_osd;
    Random & gen;
    TimeObj & timeobj;

    ClientExecute rail_client_execute;

    std::array<uint8_t, 28> server_auto_reconnect_packet {};

    ModuleIndex old_target_module = MODULE_UNKNOWN;

public:


    REDEMPTION_VERBOSE_FLAGS(private, verbose)
    {
        none,
        new_mod = 0x1,
    };

    int validator_fd = -1;

private:
    rdp_api*       rdpapi = nullptr;
    windowing_api* winapi = nullptr;

    SocketTransport * socket_transport = nullptr;

    EndSessionWarning end_session_warning;

    CryptoContext& cctx;

public:
    ModuleManager(SessionReactor& session_reactor, Front & front, Inifile & ini, CryptoContext& cctx, Random & gen, TimeObj & timeobj)
        : MMIni(session_reactor, ini)
        , front(front)
        , mod_osd(*this)
        , gen(gen)
        , timeobj(timeobj)
        , rail_client_execute(session_reactor, front, front,
            this->front.client_info.window_list_caps,
            ini.get<cfg::debug::mod_internal>() & 1)
        , verbose(static_cast<Verbose>(ini.get<cfg::debug::auth>()))
        , cctx(cctx)
    {
        this->mod = &this->no_mod;
    }

    [[nodiscard]] bool has_pending_data() const
    {
        return this->socket_transport && this->socket_transport->has_pending_data();
    }

    [[nodiscard]] SocketTransport* get_socket() const noexcept
    {
        return this->socket_transport;
    }

    void remove_mod() override
    {
        if (this->mod != &this->no_mod) {
            this->clear_osd_message();

            delete this->mod;
            this->mod = &this->no_mod;
            this->rdpapi = nullptr;
            this->winapi = nullptr;
            this->rail_module_host_mod_ptr = nullptr;
        }
    }

    ~ModuleManager() override
    {
        this->remove_mod();
    }

private:
    void set_mod(not_null_ptr<mod_api> mod, rdp_api* rdpapi = nullptr, windowing_api* winapi = nullptr)
    {
        while (this->front.keymap.nb_char_available()) {
            this->front.keymap.get_char();
        }
        while (this->front.keymap.nb_kevent_available()) {
            this->front.keymap.get_kevent();
        }

        this->clear_osd_message();

        this->mod = mod.get();
        this->rail_module_host_mod_ptr = nullptr;

        this->rdpapi = rdpapi;
        this->winapi = winapi;
    }

public:
    void new_mod(ModuleIndex target_module, AuthApi & authentifier, ReportMessageApi & report_message) override
    {
        if (target_module != MODULE_INTERNAL_TRANSITION) {
            LOG(LOG_INFO, "----------> ACL new_mod <--------");
            LOG(LOG_INFO, "target_module=%s(%d)",
                get_module_name(target_module), target_module);
        }

        this->rail_client_execute.enable_remote_program(this->front.client_info.remote_program);

        switch (target_module) {
        case MODULE_INTERNAL_CLOSE:
        case MODULE_INTERNAL_WIDGET_LOGIN:
            log_proxy::set_user("");
            break;
        default:
            log_proxy::set_user(this->ini.get<cfg::globals::auth_user>().c_str());
            break;
        }

        this->connected = false;

        if (this->old_target_module != target_module) {
            this->front.must_be_stop_capture();

            auto is_remote_mod = [](int mod_type){
                return
                    (mod_type == MODULE_XUP)
                 || (mod_type == MODULE_RDP)
                 || (mod_type == MODULE_VNC);
            };

            if (is_remote_mod(this->old_target_module)) {
                authentifier.delete_remote_mod();
            }

            if (is_remote_mod(target_module)) {
                authentifier.new_remote_mod();
            }
        }
        this->old_target_module = target_module;

        switch (target_module)
        {
        case MODULE_INTERNAL_BOUNCER2:
            LOG(LOG_INFO, "ModuleManager::Creation of internal module 'bouncer2_mod'");
            this->set_mod(new Bouncer2Mod(
                this->session_reactor,
                this->front.client_info.screen_info.width,
                this->front.client_info.screen_info.height
            ));
            LOG_IF(bool(this->verbose & Verbose::new_mod),
                LOG_INFO, "ModuleManager::internal module 'bouncer2_mod' ready");
            break;
        case MODULE_INTERNAL_TEST:
            LOG(LOG_INFO, "ModuleManager::Creation of internal module 'test'");
            this->set_mod(new ReplayMod(
                this->session_reactor,
                this->front,
                this->front,
                [this]{
                    auto movie_path = this->ini.get<cfg::video::replay_path>().as_string()
                                    + this->ini.get<cfg::globals::target_user>();
                    if (movie_path.size() < 5u || !std::equal(movie_path.end() - 5u, movie_path.end(), ".mwrm")) {
                        movie_path += ".mwrm";
                    }
                    return movie_path;
                }().c_str(),
                this->front.client_info.screen_info.width,
                this->front.client_info.screen_info.height,
                this->ini.get_ref<cfg::context::auth_error_message>(),
                !this->ini.get<cfg::mod_replay::on_end_of_data>(),
                this->ini.get<cfg::mod_replay::replay_on_loop>(),
                this->ini.get<cfg::video::play_video_with_corrupted_bitmap>(),
                to_verbose_flags(this->ini.get<cfg::debug::capture>())
            ));
            LOG_IF(bool(this->verbose & Verbose::new_mod),
                LOG_INFO, "ModuleManager::internal module 'test' ready");
            break;
        case MODULE_INTERNAL_WIDGETTEST:
            LOG(LOG_INFO, "ModuleManager::Creation of internal module 'widgettest'");
            this->set_mod(new WidgetTestMod(
                this->session_reactor,
                this->front,
                this->front.client_info.screen_info.width,
                this->front.client_info.screen_info.height,
                this->load_font()
            ));
            LOG(LOG_INFO, "ModuleManager::internal module 'widgettest' ready");
            break;
        case MODULE_INTERNAL_CARD:
            LOG(LOG_INFO, "ModuleManager::Creation of internal module 'test_card'");
            this->set_mod(new TestCardMod(
                this->session_reactor,
                this->front.client_info.screen_info.width,
                this->front.client_info.screen_info.height,
                this->load_font(),
                false
            ));
            LOG(LOG_INFO, "ModuleManager::internal module 'test_card' ready");
            break;
        case MODULE_INTERNAL_WIDGET_SELECTOR:
            LOG(LOG_INFO, "ModuleManager::Creation of internal module 'selector'");
            if (report_message.get_inactivity_timeout() != this->ini.get<cfg::globals::session_timeout>().count()) {
                report_message.update_inactivity_timeout();
            }

            this->set_mod(new SelectorMod(
                this->ini,
                this->session_reactor,
                this->front,
                this->front,
                this->front.client_info.screen_info.width,
                this->front.client_info.screen_info.height,
                this->rail_client_execute.adjust_rect(get_widget_rect(
                    this->front.client_info.screen_info.width,
                    this->front.client_info.screen_info.height,
                    this->front.client_info.cs_monitor
                )),
                this->rail_client_execute,
                this->load_font(),
                this->load_theme()
            ));
            LOG_IF(bool(this->verbose & Verbose::new_mod),
                LOG_INFO, "ModuleManager::internal module 'selector' ready");
            break;
        case MODULE_INTERNAL_CLOSE:
        case MODULE_INTERNAL_CLOSE_BACK: {
            bool const back_to_selector = (target_module == MODULE_INTERNAL_CLOSE_BACK);
            LOG(LOG_INFO, "ModuleManager::Creation of new mod 'INTERNAL::Close%s'",
                back_to_selector ? "Back" : "");

            if (this->ini.get<cfg::context::auth_error_message>().empty()) {
                this->ini.set<cfg::context::auth_error_message>(TR(trkeys::connection_ended, language(this->ini)));
            }

            this->set_mod(new FlatWabCloseMod(
                this->ini,
                this->session_reactor,
                this->front,
                this->front,
                this->front.client_info.screen_info.width,
                this->front.client_info.screen_info.height,
                this->rail_client_execute.adjust_rect(get_widget_rect(
                    this->front.client_info.screen_info.width,
                    this->front.client_info.screen_info.height,
                    this->front.client_info.cs_monitor
                )),
                this->rail_client_execute,
                this->load_font(),
                this->load_theme(),
                true,
                back_to_selector
            ));
            LOG(LOG_INFO, "ModuleManager::internal module Close%s ready",
                back_to_selector ? " Back" : "");
            break;
        }
        case MODULE_INTERNAL_TARGET:
            {
                LOG(LOG_INFO, "ModuleManager::Creation of internal module 'Interactive Target'");
                this->set_mod(new InteractiveTargetMod(
                    this->ini,
                    this->session_reactor,
                    this->front,
                    this->front,
                    this->front.client_info.screen_info.width,
                    this->front.client_info.screen_info.height,
                    this->rail_client_execute.adjust_rect(get_widget_rect(
                        this->front.client_info.screen_info.width,
                        this->front.client_info.screen_info.height,
                        this->front.client_info.cs_monitor
                    )),
                    this->rail_client_execute,
                    this->load_font(),
                    this->load_theme()
                ));
                LOG(LOG_INFO, "ModuleManager::internal module 'Interactive Target' ready");
            }
            break;
        case MODULE_INTERNAL_DIALOG_VALID_MESSAGE:
            {
                LOG(LOG_INFO, "ModuleManager::Creation of internal module 'Dialog Accept Message'");
                const char * message = this->ini.get<cfg::context::message>().c_str();
                const char * button = TR(trkeys::refused, language(this->ini));
                const char * caption = "Information";
                this->set_mod(new FlatDialogMod(
                    this->ini,
                    this->session_reactor,
                    this->front,
                    this->front,
                    this->front.client_info.screen_info.width,
                    this->front.client_info.screen_info.height,
                    this->rail_client_execute.adjust_rect(get_widget_rect(
                        this->front.client_info.screen_info.width,
                        this->front.client_info.screen_info.height,
                        this->front.client_info.cs_monitor
                    )),
                    caption,
                    message,
                    button,
                    this->rail_client_execute,
                    this->load_font(),
                    this->load_theme()
                ));
                LOG(LOG_INFO, "ModuleManager::internal module 'Dialog Accept Message' ready");
            }
            break;
        case MODULE_INTERNAL_DIALOG_DISPLAY_MESSAGE:
            {
                LOG(LOG_INFO, "ModuleManager::Creation of internal module 'Dialog Display Message'");
                const char * message = this->ini.get<cfg::context::message>().c_str();
                const char * button = nullptr;
                const char * caption = "Information";
                this->set_mod(new FlatDialogMod(
                    this->ini,
                    this->session_reactor,
                    this->front,
                    this->front,
                    this->front.client_info.screen_info.width,
                    this->front.client_info.screen_info.height,
                    this->rail_client_execute.adjust_rect(get_widget_rect(
                        this->front.client_info.screen_info.width,
                        this->front.client_info.screen_info.height,
                        this->front.client_info.cs_monitor
                    )),
                    caption,
                    message,
                    button,
                    this->rail_client_execute,
                    this->load_font(),
                    this->load_theme()
                ));
                LOG(LOG_INFO, "ModuleManager::internal module 'Dialog Display Message' ready");
            }
            break;
        case MODULE_INTERNAL_DIALOG_CHALLENGE:
            {
                LOG(LOG_INFO, "ModuleManager::Creation of internal module 'Dialog Challenge'");
                const char * message = this->ini.get<cfg::context::message>().c_str();
                const char * button = nullptr;
                const char * caption = "Challenge";
                ChallengeOpt challenge = CHALLENGE_HIDE;
                if (this->ini.get<cfg::context::authentication_challenge>()) {
                    challenge = CHALLENGE_ECHO;
                }
                this->ini.ask<cfg::context::authentication_challenge>();
                this->ini.ask<cfg::context::password>();
                this->set_mod(new FlatDialogMod(
                    this->ini,
                    this->session_reactor,
                    this->front,
                    this->front,
                    this->front.client_info.screen_info.width,
                    this->front.client_info.screen_info.height,
                    this->rail_client_execute.adjust_rect(get_widget_rect(
                        this->front.client_info.screen_info.width,
                        this->front.client_info.screen_info.height,
                        this->front.client_info.cs_monitor
                    )),
                    caption,
                    message,
                    button,
                    this->rail_client_execute,
                    this->load_font(),
                    this->load_theme(),
                    challenge
                ));
                LOG(LOG_INFO, "ModuleManager::internal module 'Dialog Challenge' ready");
            }

            break;
        case MODULE_INTERNAL_WAIT_INFO:
            {
                LOG(LOG_INFO, "ModuleManager::Creation of internal module 'Wait Info Message'");
                const char * message = this->ini.get<cfg::context::message>().c_str();
                const char * caption = TR(trkeys::information, language(this->ini));
                bool showform = this->ini.get<cfg::context::showform>();
                uint flag = this->ini.get<cfg::context::formflag>();
                this->set_mod(new FlatWaitMod(
                    this->ini,
                    this->session_reactor,
                    this->front,
                    this->front,
                    this->front.client_info.screen_info.width,
                    this->front.client_info.screen_info.height,
                    this->rail_client_execute.adjust_rect(get_widget_rect(
                        this->front.client_info.screen_info.width,
                        this->front.client_info.screen_info.height,
                        this->front.client_info.cs_monitor
                    )),
                    caption,
                    message,
                    this->rail_client_execute,
                    this->load_font(),
                    this->load_theme(),
                    showform,
                    flag
                ));
                LOG(LOG_INFO, "ModuleManager::internal module 'Wait Info Message' ready");
            }
            break;
        case MODULE_INTERNAL_TRANSITION:
            {
                this->set_mod(new TransitionMod(
                    this->ini,
                    this->session_reactor,
                    this->front,
                    this->front,
                    this->front.client_info.screen_info.width,
                    this->front.client_info.screen_info.height,
                    this->rail_client_execute.adjust_rect(get_widget_rect(
                        this->front.client_info.screen_info.width,
                        this->front.client_info.screen_info.height,
                        this->front.client_info.cs_monitor
                    )),
                    this->rail_client_execute,
                    this->load_font(),
                    this->load_theme()
                ));
                LOG(LOG_INFO, "ModuleManager::internal module 'Transition' loaded");
            }
            break;
        case MODULE_INTERNAL_WIDGET_LOGIN: {
            char username[255]; // should use string
            username[0] = 0;
            LOG(LOG_INFO, "ModuleManager::Creation of internal module 'Login'");
            if (!this->ini.is_asked<cfg::globals::auth_user>()){
                if (this->ini.is_asked<cfg::globals::target_user>()
                 || this->ini.is_asked<cfg::globals::target_device>()){
                    utils::strlcpy(
                        username,
                        this->ini.get<cfg::globals::auth_user>().c_str(),
                        sizeof(username));
                }
                else {
                    // TODO check this! Assembling parts to get user login with target is not obvious method used below il likely to show @: if target fields are empty
                    snprintf( username, sizeof(username), "%s@%s:%s%s%s"
                            , this->ini.get<cfg::globals::target_user>().c_str()
                            , this->ini.get<cfg::globals::target_device>().c_str()
                            , this->ini.get<cfg::context::target_protocol>().c_str()
                            , (!this->ini.get<cfg::context::target_protocol>().empty() ? ":" : "")
                            , this->ini.get<cfg::globals::auth_user>().c_str()
                            );
                }

                username[sizeof(username) - 1] = 0;
            }

            this->set_mod(new FlatLoginMod(
                this->ini,
                this->session_reactor,
                username,
                "", // password
                this->front,
                this->front,
                this->front.client_info.screen_info.width,
                this->front.client_info.screen_info.height,
                this->rail_client_execute.adjust_rect(get_widget_rect(
                    this->front.client_info.screen_info.width,
                    this->front.client_info.screen_info.height,
                    this->front.client_info.cs_monitor
                )),
                this->rail_client_execute,
                this->load_font(),
                this->load_theme()
            ));
            LOG(LOG_INFO, "ModuleManager::internal module Login ready");
            break;
        }

        case MODULE_XUP: {
            const char * name = "XUP Target";
            LOG(LOG_INFO, "ModuleManager::Creation of new mod 'XUP'");

            unique_fd client_sck = this->connect_to_target_host(
                report_message, trkeys::authentification_x_fail);

            this->set_mod(new ModWithSocket<xup_mod>(
                *this,
                authentifier,
                name,
                std::move(client_sck),
                this->ini.get<cfg::debug::mod_xup>(),
                nullptr,
                sock_mod_barrier(),
                this->session_reactor,
                this->front,
                this->front.client_info.screen_info.width,
                this->front.client_info.screen_info.height,
                safe_int(this->ini.get<cfg::context::opt_bpp>())
                // TODO: shouldn't alls mods have access to sesman authentifier ?
            ));

            this->ini.get_ref<cfg::context::auth_error_message>().clear();
            LOG(LOG_INFO, "ModuleManager::Creation of new mod 'XUP' suceeded");
            this->connected = true;
            break;
        }

        case MODULE_RDP:
            this->create_mod_rdp(
                authentifier, report_message, this->ini,
                this->front, this->front, this->front.client_info,
                this->rail_client_execute, this->front.keymap.key_flags,
                this->server_auto_reconnect_packet);
            break;

        case MODULE_VNC:
            this->create_mod_vnc(
                authentifier, report_message, this->ini,
                this->front, this->front, this->front.client_info,
                this->rail_client_execute, this->front.keymap.key_flags);
            break;

        default:
            LOG(LOG_INFO, "ModuleManager::Unknown backend exception");
            throw Error(ERR_SESSION_UNKNOWN_BACKEND);
        }
    }

    [[nodiscard]] rdp_api* get_rdp_api() const override {
        return this->rdpapi;
    }

    void update_end_session_warning(time_t start_time, time_t end_time, time_t now) {
        std::string mes;
        this->end_session_warning.update_osd_state(mes, language(this->ini), start_time, end_time, now);
        if (!mes.empty()) {
            this->osd_message(std::move(mes), true);
        }
    }

private:
    unique_fd connect_to_target_host(ReportMessageApi& report_message, trkeys::TrKey const& authentification_fail)
    {
        auto throw_error = [this, &report_message](char const* error_message, int id) {
            LOG_PROXY_SIEM("TARGET_CONNECTION_FAILED",
                R"(target="%s" host="%s" port="%u" reason="%s")",
                this->ini.get<cfg::globals::target_user>(),
                this->ini.get<cfg::context::target_host>(),
                this->ini.get<cfg::context::target_port>(),
                error_message);

            report_message.log6(LogId::CONNECTION_FAILED, this->session_reactor.get_current_time(), {});

            this->ini.set<cfg::context::auth_error_message>(TR(trkeys::target_fail, language(this->ini)));

            LOG(LOG_ERR, "%s", (id == 1)
                ? "Failed to connect to remote TCP host (1)"
                : "Failed to connect to remote TCP host (2)");
            throw Error(ERR_SOCKET_CONNECT_FAILED);
        };

        LOG_PROXY_SIEM("TARGET_CONNECTION",
            R"(target="%s" host="%s" port="%u")",
            this->ini.get<cfg::globals::target_user>(),
            this->ini.get<cfg::context::target_host>(),
            this->ini.get<cfg::context::target_port>());

        const char * ip = this->ini.get<cfg::context::target_host>().c_str();
        char ip_addr[256] {};
        in_addr s4_sin_addr;
        if (auto error_message = resolve_ipv4_address(ip, s4_sin_addr)) {
            // TODO: actually this is DNS Failure or invalid address
            throw_error(error_message, 1);
        }

        snprintf(ip_addr, sizeof(ip_addr), "%s", inet_ntoa(s4_sin_addr));

        char const* error_message = nullptr;
        unique_fd client_sck = ip_connect(ip, this->ini.get<cfg::context::target_port>(), &error_message);

        if (!client_sck.is_open()) {
            throw_error(error_message, 2);
        }

        this->ini.set<cfg::context::auth_error_message>(TR(authentification_fail, language(this->ini)));
        this->ini.set<cfg::context::ip_target>(ip_addr);

        return client_sck;
    }



    void create_mod_rdp(
        AuthApi& authentifier, ReportMessageApi& report_message,
        Inifile& ini, gdi::GraphicApi & drawable, FrontAPI& front, ClientInfo client_info,
        ClientExecute& rail_client_execute, Keymap2::KeyFlags key_flags,
        std::array<uint8_t, 28>& server_auto_reconnect_packet);

    void create_mod_vnc(
        AuthApi& authentifier, ReportMessageApi& report_message,
        Inifile& ini, gdi::GraphicApi & drawable, FrontAPI& front, ClientInfo const& client_info,
        ClientExecute& rail_client_execute, Keymap2::KeyFlags key_flags);

    Font& load_font()
    {
        if (this->_font_is_loaded) {
            return this->_font;
        }

        this->_font = Font(
            app_path(AppPath::DefaultFontFile),
            ini.get<cfg::globals::spark_view_specific_glyph_width>());

        this->_font_is_loaded = true;
        return this->_font;
    }

    Theme& load_theme()
    {
        if (this->_theme_is_loaded) {
            return this->_theme;
        }

        auto & theme_name = this->ini.get<cfg::internal_mod::theme>();
        LOG_IF(this->ini.get<cfg::debug::config>(), LOG_INFO, "LOAD_THEME: %s", theme_name);

        ::load_theme(this->_theme, theme_name);

        this->_theme_is_loaded = true;
        return this->_theme;
    }

    Theme _theme;
    Font _font;
    bool _theme_is_loaded = false;
    bool _font_is_loaded = false;
};
