//
// Automated Testing Framework (atf)
//
// Copyright (c) 2007, 2008, 2010 The NetBSD Foundation, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
// CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

extern "C" {
#include <sys/stat.h>
}

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "atf-c++/env.hpp"
#include "atf-c++/fs.hpp"
#include "atf-c++/macros.hpp"
#include "atf-c++/process.hpp"

// ------------------------------------------------------------------------
// Auxiliary functions.
// ------------------------------------------------------------------------

static
void
touch(const std::string& path)
{
    std::ofstream os(path.c_str());
    if (!os)
        ATF_FAIL("Could not create file " + path);
    os.close();
}

// ------------------------------------------------------------------------
// Helper tests for "t_integration".
// ------------------------------------------------------------------------

ATF_TEST_CASE(config);
ATF_TEST_CASE_HEAD(config)
{
    set_md_var("descr", "Helper test case for the t_integration test program");
}
ATF_TEST_CASE_BODY(config)
{
    std::cout << "1st: " << get_config_var("1st") << std::endl;
    std::cout << "2nd: " << get_config_var("2nd") << std::endl;
    std::cout << "3rd: " << get_config_var("3rd") << std::endl;
    std::cout << "4th: " << get_config_var("4th") << std::endl;
}

ATF_TEST_CASE(fds);
ATF_TEST_CASE_HEAD(fds)
{
    set_md_var("descr", "Helper test case for the t_integration test program");
}
ATF_TEST_CASE_BODY(fds)
{
    std::cout << "msg1 to stdout" << std::endl;
    std::cout << "msg2 to stdout" << std::endl;
    std::cerr << "msg1 to stderr" << std::endl;
    std::cerr << "msg2 to stderr" << std::endl;
}

ATF_TEST_CASE(testvar);
ATF_TEST_CASE_HEAD(testvar)
{
    set_md_var("descr", "Helper test case for the t_integration test program");
}
ATF_TEST_CASE_BODY(testvar)
{
    if (!has_config_var("testvar"))
        atf_tc_fail("testvar variable not defined");
    std::cout << "testvar: " << get_config_var("testvar") << std::endl;
}

ATF_TEST_CASE(env_list);
ATF_TEST_CASE_HEAD(env_list)
{
    set_md_var("descr", "Helper test case for the t_integration test program");
}
ATF_TEST_CASE_BODY(env_list)
{
    const atf::process::status s =
        atf::process::exec(atf::fs::path("env"),
                           atf::process::argv_array("env", NULL),
                           atf::process::stream_inherit(),
                           atf::process::stream_inherit());
    ATF_CHECK(s.exited());
    ATF_CHECK(s.exitstatus() == EXIT_SUCCESS);
}

ATF_TEST_CASE(env_home);
ATF_TEST_CASE_HEAD(env_home)
{
    set_md_var("descr", "Helper test case for the t_integration test program");
}
ATF_TEST_CASE_BODY(env_home)
{
    ATF_CHECK(atf::env::has("HOME"));
    atf::fs::path p(atf::env::get("HOME"));
    atf::fs::file_info fi1(p);
    atf::fs::file_info fi2(atf::fs::get_current_dir());
    ATF_CHECK_EQUAL(fi1.get_device(), fi2.get_device());
    ATF_CHECK_EQUAL(fi1.get_inode(), fi2.get_inode());
}

ATF_TEST_CASE(umask);
ATF_TEST_CASE_HEAD(umask)
{
    set_md_var("descr", "Helper test case for the t_integration test program");
}
ATF_TEST_CASE_BODY(umask)
{
    mode_t m = ::umask(0);
    std::cout << "umask: " << std::setw(4) << std::setfill('0')
              << std::oct << m << std::endl;
    (void)::umask(m);
}

ATF_TEST_CASE_WITH_CLEANUP(cleanup_states);
ATF_TEST_CASE_HEAD(cleanup_states)
{
    set_md_var("descr", "Helper test case for the t_integration test program");
}
ATF_TEST_CASE_BODY(cleanup_states)
{
    touch(get_config_var("statedir") + "/to-delete");
    touch(get_config_var("statedir") + "/to-stay");

    if (get_config_var("state") == "fail")
        ATF_FAIL("On purpose");
    else if (get_config_var("state") == "skip")
        ATF_SKIP("On purpose");
}
ATF_TEST_CASE_CLEANUP(cleanup_states)
{
    atf::fs::remove(atf::fs::path(get_config_var("statedir") + "/to-delete"));
}

ATF_TEST_CASE_WITH_CLEANUP(cleanup_curdir);
ATF_TEST_CASE_HEAD(cleanup_curdir)
{
    set_md_var("descr", "Helper test case for the t_integration test program");
}
ATF_TEST_CASE_BODY(cleanup_curdir)
{
    std::ofstream os("oldvalue");
    if (!os)
        ATF_FAIL("Failed to create oldvalue file");
    os << 1234;
    os.close();
}
ATF_TEST_CASE_CLEANUP(cleanup_curdir)
{
    std::ifstream is("oldvalue");
    if (is) {
        int i;
        is >> i;
        std::cout << "Old value: " << i << std::endl;
        is.close();
    }
}

ATF_TEST_CASE(require_arch);
ATF_TEST_CASE_HEAD(require_arch)
{
    set_md_var("descr", "Helper test case for the t_integration test program");
    set_md_var("require.arch", get_config_var("arch", "not-set"));
}
ATF_TEST_CASE_BODY(require_arch)
{
}

ATF_TEST_CASE(require_config);
ATF_TEST_CASE_HEAD(require_config)
{
    set_md_var("descr", "Helper test case for the t_integration test program");
    set_md_var("require.config", "var1 var2");
}
ATF_TEST_CASE_BODY(require_config)
{
    std::cout << "var1: " << get_config_var("var1") << std::endl;
    std::cout << "var2: " << get_config_var("var2") << std::endl;
}

ATF_TEST_CASE(require_machine);
ATF_TEST_CASE_HEAD(require_machine)
{
    set_md_var("descr", "Helper test case for the t_integration test program");
    set_md_var("require.machine", get_config_var("machine", "not-set"));
}
ATF_TEST_CASE_BODY(require_machine)
{
}

ATF_TEST_CASE(require_progs);
ATF_TEST_CASE_HEAD(require_progs)
{
    set_md_var("descr", "Helper test case for the t_integration test program");
    set_md_var("require.progs", get_config_var("progs", "not-set"));
}
ATF_TEST_CASE_BODY(require_progs)
{
}

ATF_TEST_CASE(require_user);
ATF_TEST_CASE_HEAD(require_user)
{
    set_md_var("descr", "Helper test case for the t_integration test program");
    set_md_var("require.user", get_config_var("user", "not-set"));
}
ATF_TEST_CASE_BODY(require_user)
{
}

// ------------------------------------------------------------------------
// Main.
// ------------------------------------------------------------------------

ATF_INIT_TEST_CASES(tcs)
{
    std::string which = atf::env::get("TESTCASE");

    // Add helper tests for t_integration.
    if (which == "config")
        ATF_ADD_TEST_CASE(tcs, config);
    if (which == "fds")
        ATF_ADD_TEST_CASE(tcs, fds);
    if (which == "testvar")
        ATF_ADD_TEST_CASE(tcs, testvar);
    if (which == "env_list")
        ATF_ADD_TEST_CASE(tcs, env_list);
    if (which == "env_home")
        ATF_ADD_TEST_CASE(tcs, env_home);
    if (which == "umask")
        ATF_ADD_TEST_CASE(tcs, umask);
    if (which == "cleanup_states")
        ATF_ADD_TEST_CASE(tcs, cleanup_states);
    if (which == "cleanup_curdir")
        ATF_ADD_TEST_CASE(tcs, cleanup_curdir);
    if (which == "require_arch")
        ATF_ADD_TEST_CASE(tcs, require_arch);
    if (which == "require_config")
        ATF_ADD_TEST_CASE(tcs, require_config);
    if (which == "require_machine")
        ATF_ADD_TEST_CASE(tcs, require_machine);
    if (which == "require_progs")
        ATF_ADD_TEST_CASE(tcs, require_progs);
    if (which == "require_user")
        ATF_ADD_TEST_CASE(tcs, require_user);
}
