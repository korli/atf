//
// Automated Testing Framework (atf)
//
// Copyright (c) 2007 The NetBSD Foundation, Inc.
// All rights reserved.
//
// This code is derived from software contributed to The NetBSD Foundation
// by Julio M. Merino Vidal, developed as part of Google's Summer of Code
// 2007 program.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this
//    software must display the following acknowledgement:
//        This product includes software developed by the NetBSD
//        Foundation, Inc. and its contributors.
// 4. Neither the name of The NetBSD Foundation nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
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
#include <unistd.h>
}

#include <cassert>
#include <cerrno>

#include "atfprivate/exceptions.hpp"
#include "atfprivate/io.hpp"

namespace impl = atf::io;
#define IMPL_NAME "atf::io"

// ------------------------------------------------------------------------
// The "file_handle" class.
// ------------------------------------------------------------------------

impl::file_handle::file_handle(void) :
    m_handle(invalid_value())
{
}

impl::file_handle::file_handle(handle_type h) :
    m_handle(h)
{
    assert(m_handle != invalid_value());
}

impl::file_handle::file_handle(const file_handle& fh) :
    m_handle(fh.m_handle)
{
    fh.m_handle = invalid_value();
}

impl::file_handle::~file_handle(void)
{
    if (is_valid())
        close();
}

impl::file_handle&
impl::file_handle::operator=(const file_handle& fh)
{
    m_handle = fh.m_handle;
    fh.m_handle = invalid_value();

    return *this;
}

bool
impl::file_handle::is_valid(void)
    const
{
    return m_handle != invalid_value();
}

void
impl::file_handle::close(void)
{
    assert(is_valid());

    ::close(m_handle);

    m_handle = invalid_value();
}

impl::file_handle::handle_type
impl::file_handle::disown(void)
{
    assert(is_valid());

    handle_type h = m_handle;
    m_handle = invalid_value();
    return h;
}

impl::file_handle::handle_type
impl::file_handle::get(void)
    const
{
    assert(is_valid());

    return m_handle;
}

void
impl::file_handle::posix_remap(handle_type h)
{
    assert(is_valid());

    if (m_handle == h)
        return;

    if (::dup2(m_handle, h) == -1)
        throw system_error(IMPL_NAME "::file_handle::posix_remap",
                           "dup2(2) failed", errno);

    if (::close(m_handle) == -1) {
        ::close(h);
        throw system_error(IMPL_NAME "::file_handle::posix_remap",
                           "close(2) failed", errno);
    }

    m_handle = h;
}

impl::file_handle
impl::file_handle::posix_dup(int h1)
{
    int h2 = ::dup(h1);
    if (h2 == -1)
        throw system_error(IMPL_NAME "::file_handle::posix_dup",
                           "dup2(2) failed", errno);

    return file_handle(h2);
}

impl::file_handle
impl::file_handle::posix_dup(int h1, int h2)
{
    if (::dup2(h1, h2) == -1)
        throw system_error(IMPL_NAME "::file_handle::posix_dup",
                           "dup2(2) failed", errno);

    return file_handle(h2);
}

const impl::file_handle::handle_type
impl::file_handle::invalid_value(void)
{
    return -1;
}

// ------------------------------------------------------------------------
// The "systembuf" class.
// ------------------------------------------------------------------------

impl::systembuf::systembuf(handle_type h, std::size_t bufsize) :
    m_handle(h),
    m_bufsize(bufsize),
    m_read_buf(NULL),
    m_write_buf(NULL)
{
    assert(m_handle >= 0);
    assert(m_bufsize > 0);

    try {
        m_read_buf = new char[bufsize];
        m_write_buf = new char[bufsize];
    } catch (...) {
        if (m_read_buf != NULL)
            delete [] m_read_buf;
        if (m_write_buf != NULL)
            delete [] m_write_buf;
        throw;
    }

    setp(m_write_buf, m_write_buf + m_bufsize);
}

impl::systembuf::~systembuf(void)
{
    delete [] m_read_buf;
    delete [] m_write_buf;
}

impl::systembuf::int_type
impl::systembuf::underflow(void)
{
    assert(gptr() >= egptr());

    bool ok;
    ssize_t cnt = ::read(m_handle, m_read_buf, m_bufsize);
    ok = (cnt != -1 && cnt != 0);

    if (!ok)
        return traits_type::eof();
    else {
        setg(m_read_buf, m_read_buf, m_read_buf + cnt);
        return traits_type::to_int_type(*gptr());
    }
}

impl::systembuf::int_type
impl::systembuf::overflow(int c)
{
    assert(pptr() >= epptr());
    if (sync() == -1)
        return traits_type::eof();
    if (!traits_type::eq_int_type(c, traits_type::eof())) {
        traits_type::assign(*pptr(), c);
        pbump(1);
    }
    return traits_type::not_eof(c);
}

int
impl::systembuf::sync(void)
{
    ssize_t cnt = pptr() - pbase();

    bool ok;
    ok = ::write(m_handle, pbase(), cnt) == cnt;

    if (ok)
        pbump(-cnt);
    return ok ? 0 : -1;
}

// ------------------------------------------------------------------------
// The "pipe" class.
// ------------------------------------------------------------------------

impl::pipe::pipe(void)
{
    file_handle::handle_type hs[2];

    if (::pipe(hs) == -1)
        throw system_error(IMPL_NAME "::pipe::pipe",
                           "pipe(2) failed", errno);

    m_read_end = file_handle(hs[0]);
    m_write_end = file_handle(hs[1]);
}

impl::file_handle&
impl::pipe::rend(void)
{
    return m_read_end;
}

impl::file_handle&
impl::pipe::wend(void)
{
    return m_write_end;
}

// ------------------------------------------------------------------------
// The "pistream" class.
// ------------------------------------------------------------------------

impl::pistream::pistream(impl::file_handle& fh) :
    std::istream(NULL),
    m_handle(fh),
    m_systembuf(m_handle.get())
{
    rdbuf(&m_systembuf);
}

void
impl::pistream::close(void)
{
    m_handle.close();
}

// ------------------------------------------------------------------------
// The "postream" class.
// ------------------------------------------------------------------------

impl::postream::postream(impl::file_handle& fh) :
    std::ostream(NULL),
    m_handle(fh),
    m_systembuf(m_handle.get())
{
    rdbuf(&m_systembuf);
}

void
impl::postream::close(void)
{
    m_handle.close();
}
