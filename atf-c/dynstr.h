/*
 * Automated Testing Framework (atf)
 *
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if !defined(ATF_C_DYNSTR_H)
#define ATF_C_DYNSTR_H

#include <stdarg.h>

#include <atf-c/error.h>
#include <atf-c/object.h>

/* ---------------------------------------------------------------------
 * The "atf_dynstr" type.
 * --------------------------------------------------------------------- */

struct atf_dynstr {
    atf_object_t m_object;

    char *m_data;
    size_t m_datasize;
    size_t m_length;
};
typedef struct atf_dynstr atf_dynstr_t;

/* Constants */
extern const size_t atf_dynstr_npos;

/* Constructors and destructors */
atf_error_t atf_dynstr_init(atf_dynstr_t *);
atf_error_t atf_dynstr_init_ap(atf_dynstr_t *, const char *, va_list);
atf_error_t atf_dynstr_init_fmt(atf_dynstr_t *, const char *, ...);
atf_error_t atf_dynstr_init_raw(atf_dynstr_t *, const void *, size_t);
atf_error_t atf_dynstr_init_rep(atf_dynstr_t *, size_t, char);
atf_error_t atf_dynstr_init_substr(atf_dynstr_t *, const atf_dynstr_t *,
                                   size_t, size_t);
void atf_dynstr_fini(atf_dynstr_t *);
char *atf_dynstr_fini_disown(atf_dynstr_t *);

/* Getters */
const char *atf_dynstr_cstring(const atf_dynstr_t *);
size_t atf_dynstr_length(atf_dynstr_t *);
size_t atf_dynstr_rfind_ch(const atf_dynstr_t *, char);

/* Modifiers */
atf_error_t atf_dynstr_append_ap(atf_dynstr_t *, const char *, va_list);
atf_error_t atf_dynstr_append_fmt(atf_dynstr_t *, const char *, ...);
void atf_dynstr_clear(atf_dynstr_t *);
atf_error_t atf_dynstr_prepend_ap(atf_dynstr_t *, const char *, va_list);
atf_error_t atf_dynstr_prepend_fmt(atf_dynstr_t *, const char *, ...);

/* Operators */
bool atf_equal_dynstr_cstring(const atf_dynstr_t *, const char *);

#endif /* ATF_C_DYNSTR_H */
