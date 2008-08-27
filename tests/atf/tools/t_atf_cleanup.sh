#
# Automated Testing Framework (atf)
#
# Copyright (c) 2007, 2008 The NetBSD Foundation, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
# CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

cleanup="$(atf-config -t atf_libexecdir)/atf-cleanup"

atf_test_case file
file_head()
{
    atf_set "descr" "Tests that a single file can be removed"
}
file_body()
{
    atf_check -s eq:0 -o empty -e empty touch foo
    atf_check -s eq:0 -o empty -e empty "${cleanup}" foo
    atf_check -s eq:1 -o empty -e empty test -e foo
}

atf_test_case dir_empty
dir_empty_head()
{
    atf_set "descr" "Tests that a single empty directory can be removed"
}
dir_empty_body()
{
    atf_check -s eq:0 -o empty -e empty mkdir foo
    atf_check -s eq:0 -o empty -e empty "${cleanup}" foo
    atf_check -s eq:1 -o empty -e empty test -e foo
}

atf_test_case dir_full
dir_full_head()
{
    atf_set "descr" "Tests that a directory with contents can be removed"
}
dir_full_body()
{
    atf_check -s eq:0 -o empty -e empty mkdir foo
    atf_check -s eq:0 -o empty -e empty mkdir foo/bar
    atf_check -s eq:0 -o empty -e empty touch foo/bar/baz
    atf_check -s eq:0 -o empty -e empty touch foo/baz
    atf_check -s eq:0 -o empty -e empty "${cleanup}" foo
    atf_check -s eq:1 -o empty -e empty test -e foo
}

atf_test_case mount
mount_head()
{
    atf_set "descr" "Tests that the removal algorithm does not cross" \
                    "mount points"
    atf_set "require.user" "root"
}
mount_body()
{
    platform=$(uname)
    case ${platform} in
    Linux|FreeBSD|NetBSD|SunOS)
        mount_cmd="$(atf_get_srcdir)/h_mount_tmpfs.sh"
        mkdir foo
        mkdir foo/bar
        mkdir foo/bar/mnt
        atf_check -s eq:0 -o empty -e empty -x "${mount_cmd} foo/bar/mnt"
        mkdir foo/baz
        atf_check -s eq:0 -o empty -e empty -x "${mount_cmd} foo/baz"
        mkdir foo/baz/foo
        mkdir foo/baz/foo/bar
        atf_check -s eq:0 -o empty -e empty -x \
                  "${mount_cmd} foo/baz/foo/bar"
        ;;
    *)
        # XXX Possibly specify in meta-data too.
        atf_skip "Test unimplemented in this platform (${platform})"
        ;;
    esac

    atf_check -s eq:0 -o empty -e empty "${cleanup}" foo
    mount | grep $(pwd)/foo && \
        atf_fail "Some file systems remain mounted"
    atf_check -s eq:1 -o empty -e empty test -d foo
}

atf_test_case symlink
symlink_head()
{
    atf_set "descr" "Tests that the removal algorithm does not follow" \
                    "symlinks, which may live in another device and thus" \
                    "be treated as mount points"
    atf_set "require.user" "root"
}
symlink_body()
{
    platform=$(uname)
    case ${platform} in
    Linux|FreeBSD|NetBSD|SunOS)
        mount_cmd="$(atf_get_srcdir)/h_mount_tmpfs.sh"
        atf_check -s eq:0 -o empty -e empty mkdir foo
        atf_check -s eq:0 -o empty -e empty mkdir foo/bar
        atf_check -s eq:0 -o empty -e empty -x "${mount_cmd} foo/bar"
        atf_check -s eq:0 -o empty -e empty touch a
        atf_check -s eq:0 -o empty -e empty ln -s "$(pwd)/a" foo/bar
        ;;
    *)
        # XXX Possibly specify in meta-data too.
        atf_skip "Test unimplemented in this platform (${platform})"
        ;;
    esac

    atf_check -s eq:0 -o empty -e empty ${cleanup} foo
    mount | grep $(pwd)/foo && \
        atf_fail "Some file systems remain mounted"
    atf_check -s eq:1 -o empty -e empty test -d foo
}

atf_init_test_cases()
{
    atf_add_test_case file
    atf_add_test_case dir_empty
    atf_add_test_case dir_full
    atf_add_test_case mount
    atf_add_test_case symlink
}

# vim: syntax=sh:expandtab:shiftwidth=4:softtabstop=4
