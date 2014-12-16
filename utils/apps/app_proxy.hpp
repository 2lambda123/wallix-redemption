/*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*   Product name: redemption, a FLOSS RDP proxy
*   Copyright (C) Wallix 2010-2014
*   Author(s): Jonathan Poelen
*/

#ifndef REDEMPTION_UTILS_APPS_APP_PROXY_HPP
#define REDEMPTION_UTILS_APPS_APP_PROXY_HPP

#include <signal.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stddef.h>
#include <unistd.h>

#include <iostream>
#include <cerrno>

#include "openssl_crypto.hpp"

#include "config.hpp"
#include "check_files.hpp"
#include "mainloop.hpp"
#include "log.hpp"
#include "fdbuf.hpp"

#include "version.hpp"

inline void daemonize(const char * pid_file)
{
    int pid;
    int fd;
    char text[256];
    size_t lg;

    using namespace std;

    close(0);
    close(1);
    close(2);

    switch (pid = fork()){
    case -1:
        std::clog << "problem forking "
        << errno << ":'" << strerror(errno) << "'\n";
        _exit(1);
    default: /* exit, this is the main process (daemonizer) */
        _exit(0);
    case 0: /* child daemon process */
        pid = getpid();
        fd = open(pid_file, O_WRONLY | O_CREAT, S_IRWXU);
        if (fd == -1) {
            std::clog
            <<  "Writing process id to " LOCKFILE " failed. Maybe no rights ?"
            << " : " << errno << ":'" << strerror(errno) << "'\n";
            _exit(1);
        }
        lg = snprintf(text, 255, "%d", pid);
        if (io::posix::fdbuf(fd).write(text, lg) == -1) {
            LOG(LOG_ERR, "Couldn't write pid to %s: %s", pid_file, strerror(errno));
            _exit(1);
        }
        usleep(1000000);
        open("/dev/null", O_RDONLY, 0); // stdin  0
        open("/dev/null", O_WRONLY, 0); // stdout 1
        open("/dev/null", O_WRONLY, 0); // stderr 2

        std::clog << "Process " << pid << " started as daemon\n";
    }
}


/*****************************************************************************/
inline int shutdown(const char * pid_file)
{
    int fd;
    char text[256];

    using namespace std;

    text[0] = 0;
    cout << "stopping rdpproxy\n";
    /* read the rdpproxy.pid file */
    fd = -1;
    cout << "looking if pid_file " << pid_file <<  " exists\n";
    if ((0 == access(pid_file, F_OK))) {
        fd =  open(pid_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            /* can't open read / write, try to open read only */
            fd =  open(pid_file, O_RDONLY);
        }
    }
    if (fd == -1) {
        return 1; // file does not exist.
    }
    cout << "reading pid_file " << pid_file << "\n";
    memset(text, 0, 32);
    if (read(fd, text, 31) < 0){
        cout << "failed to read pid file\n";
    }
    else{
        int pid = atoi(text);
        cout << "stopping process id " << pid << "\n";
        if (pid > 0) {
            int res = kill(pid, SIGTERM);
            if (res != -1){
                sleep(2);
                res = kill(pid,0);
            }
            if ((errno != ESRCH) || (res == 0)){
                // errno != ESRCH, pid is still running
                cout << "process " << pid << " is still running, "
                "let's send a KILL signal" << "\n";
                res = kill(pid, SIGKILL);
                if (res != -1){
                    sleep(1);
                    res = kill(pid,0);
                }
                if ((errno != ESRCH) || (res == 0)){
                    // if errno != ESRCH, pid is still running
                    cout << "Error stopping process id " << pid << "\n";
                }
            }
        }
    }
    close(fd);
    unlink(pid_file);

    // remove all other pid files
    DIR * d = opendir("/var/run/redemption");
    if (d){
        size_t path_len = strlen("/var/run/redemption/");
        size_t file_len = pathconf("/var/run/redemption/", _PC_NAME_MAX) + 1;
        char * buffer = (char*)malloc(file_len + path_len);
        strcpy(buffer, "/var/run/redemption/");
        size_t len = offsetof(struct dirent, d_name) + file_len;
        struct dirent * entryp = (struct dirent *)malloc(len);
        struct dirent * result;
        for (readdir_r(d, entryp, &result) ; result ; readdir_r(d, entryp, &result)) {
            if ((0 == strcmp(entryp->d_name, ".")) || (0 == strcmp(entryp->d_name, ".."))){
                continue;
            }
            strcpy(buffer + path_len, entryp->d_name);
            struct stat st;
            if (stat(buffer, &st) < 0){
                LOG(LOG_INFO, "Failed to read pid directory %s [%u: %s]",
                    buffer, errno, strerror(errno));
                continue;
            }
            LOG(LOG_INFO, "removing old pid file %s", buffer);
            if (unlink(buffer) < 0){
                LOG(LOG_INFO, "Failed to remove old session pid file %s [%u: %s]",
                    buffer, errno, strerror(errno));
            }
        }
        closedir(d);
        free(entryp);
        free(buffer);
    }
    else {
        LOG(LOG_INFO, "Failed to open dynamic configuration directory %s [%u: %s]",
            "/var/run/redemption" , errno, strerror(errno));
    }

    return 0;
}


#include <boost/program_options.hpp>

namespace po = boost::program_options;

struct extra_option {
    const char * option;
    const char * description;
};
using extra_option_list = std::initializer_list<extra_option>;

// ExtraOption = extra_option container
// ExtracOptionChecker = int(po::variables_map &,  bool * quit)
// EnableTransparent = int(unsigned gid, unsigned uid)
template<class CryptoKeyHldr, class ExtraOption, class ExtracOptionChecker>
int app_proxy(
    int argc, char** argv, const char * copyright_notice
  , ExtraOption const & extrax_options, ExtracOptionChecker extrac_options_checker
) {
    setlocale(LC_CTYPE, "C");

    const unsigned uid = getuid();
    const unsigned gid = getgid();

    unsigned euid = uid;
    unsigned egid = gid;

    po::options_description desc("Options");
    desc.add_options()
    // -help, --help, -h
    ("help,h", "produce help message")
    // -version, --version, -v
    ("version,v", "show software version")
    // -kill, --kill, -k
    ("kill,k", "shut down rdpproxy")
    // -nodaemon" -n
    ("nodaemon,n", "don't fork into background")

    ("uid,u", po::value<unsigned>(&euid), "run with given uid")

    ("gid,g", po::value<unsigned>(&egid), "run with given gid")

//    ("trace,t", "trace behaviour")

    ("check,c", "check installation files")

    ("force,f", "remove application lock file")

    // -inetd, --inetd, -i
    ("inetd,i", "launch redemption with inetd like launcher")

//    ("test", "check Inifile syntax")
    ;

    for (extra_option const & extra : extrax_options) {
        desc.add_options()(extra.option, extra.description);
    }

    using namespace std;

    po::variables_map options;
    po::store(po::parse_command_line(argc, argv, desc), options);
    po::notify(options);

    if (options.count("kill")) {
        int status = shutdown(PID_PATH "/redemption/" LOCKFILE);
        if (status){
            TODO("check the real error that occured")
            std::clog << "problem opening " << PID_PATH "/redemption/" LOCKFILE  << "."
            " Maybe rdpproxy is not running\n";
        }
        return status;
    }
    if (options.count("help")) {
        cout << copyright_notice << "\n\n";
        cout << "Usage: rdpproxy [options]\n\n";
        cout << desc << endl;
        return 0;
    }
    if (options.count("version")) {
        cout << copyright_notice << std::endl;
        return 0;
    }

    {
        bool quit = false;
        if (int status = extrac_options_checker(options, &quit)) {
            return status;
        }
        else if (quit) {
            return 0;
        }
    }

    openlog("rdpproxy", LOG_CONS | LOG_PERROR, LOG_USER);

    if (options.count("check")) {
        bool user_check_file_result  =
            ((uid != euid) || (gid != egid)) ?
            CheckFile::check(user_check_file_list) : true;
        /*
          setgid(egid);
          setuid(euid);
        */
        bool euser_check_file_result = CheckFile::check(euser_check_file_list);
        /*
          setgid(gid);
          setuid(uid);
        */

        if ((uid != euid) || (gid != egid)) {
            CheckFile::ShowAll(user_check_file_list, uid, gid);
        }
        CheckFile::ShowAll(euser_check_file_list, euid, egid);

        if (!user_check_file_result || !euser_check_file_result)
        {
            if ((uid != euid) || (gid != egid))
            {
                CheckFile::ShowErrors(user_check_file_list, uid, gid);
            }
            CheckFile::ShowErrors(euser_check_file_list, euid, egid);

            LOG(LOG_INFO,
                "Please verify that all tests passed. If not, "
                    "you may need to remove " PID_PATH "/redemption/"
                    LOCKFILE " or reinstall rdpproxy if some configuration "
                    "files are missing.");
        }
        return 0;
    }

    if (options.count("inetd")) {
        redemption_new_session();
        return 0;
    }

    // if -f (force option) is set
    // force kill running rdpproxy
    // force remove pid file
    // don't check if it fails (proxy may be allready stopped)
    // and try to continue normal start process afterward

    if (mkdir(PID_PATH "/redemption", 0700) < 0){
        TODO("check only for relevant errors (exists with expected permissions is OK)");
    }

    if (chown(PID_PATH "/redemption", euid, egid) < 0){
        LOG(LOG_INFO, "Failed to set owner %u.%u to " PID_PATH "/redemption", euid, egid);
        return 1;
    }

    if (options.count("force")){
        shutdown(PID_PATH  "/redemption/" LOCKFILE);
    }

    if (0 == access(PID_PATH "/redemption/" LOCKFILE, F_OK)) {
        std::clog <<
        "File " << PID_PATH "/redemption/" LOCKFILE << " already exists. "
        "It looks like rdpproxy is already running, "
        "if not, try again with -f (force) option or delete the " PID_PATH "/redemption/" LOCKFILE " file and try again\n";
        return 1;
    }


    /* write the pid to file */
    int fd = open(PID_PATH "/redemption/" LOCKFILE, O_WRONLY | O_CREAT, S_IRWXU);
    if (fd == -1) {
        std::clog
        <<  "Writing process id to " PID_PATH "/redemption/" LOCKFILE " failed. Maybe no rights ?"
        << " : " << errno << ":'" << strerror(errno) << "'\n";
        return 1;
    }
    {
        io::posix::fdbuf file(fd);
        const int pid = getpid();
        char text[256];
        size_t lg = snprintf(text, 255, "%d", pid);
        if (file.write(text, lg) == -1) {
            LOG(LOG_ERR, "Couldn't write pid to %s: %s", PID_PATH "/redemption/" LOCKFILE, strerror(errno));
            return 1;
        }
    }

    if (!options.count("nodaemon")) {
        daemonize(PID_PATH "/redemption/" LOCKFILE);
    }

    Inifile ini;
    ConfigurationLoader cfg_loader(ini, CFG_PATH "/" RDPPROXY_INI);

    OpenSSL_add_all_digests();

    if (!ini.globals.enable_ip_transparent) {
        if (setgid(egid) != 0){
            LOG(LOG_WARNING, "Changing process group to %u failed with error: %s\n", gid, strerror(errno));
            return 1;
        }
        if (setuid(euid) != 0){
            LOG(LOG_WARNING, "Changing process group to %u failed with error: %s\n", gid, strerror(errno));
            return 1;
        }
    }

    CryptoKeyHldr cryptoKeyHldr;

    LOG(LOG_INFO, "ReDemPtion " VERSION " starting");
    redemption_main_loop(ini, euid, egid, cryptoKeyHldr);

    /* delete the .pid file if it exists */
    /* don't care about errors. */
    /* If we are not in daemon mode this file will not exists, */
    /* hence some errors are expected */
    unlink(PID_PATH "/redemption/" LOCKFILE);

    return 0;
}

#endif
