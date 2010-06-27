#
# Automated Testing Framework (atf)
#
# Copyright (c) 2007, 2008, 2009, 2010 The NetBSD Foundation, Inc.
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

bin_PROGRAMS += atf-version/atf-version
atf_version_atf_version_SOURCES = atf-version/atf-version.cpp \
                                  atf-version/revision.h
atf_version_atf_version_LDADD = libatf-c++.la
dist_man_MANS += atf-version/atf-version.1

BUILT_SOURCES += atf-version/revision.h
atf-version/revision.h: admin/revision.h $(srcdir)/admin/revision-dist.h
	@$(top_srcdir)/admin/choose-revision.sh \
	    admin/revision.h $(srcdir)/admin/revision-dist.h \
	    atf-version/revision.h
CLEANFILES += atf-version/revision.h

hooksdir = $(pkgdatadir)
hooks_DATA = atf-run/share/atf-run.hooks
EXTRA_DIST += $(hooks_DATA)

egdir = $(atf_egdir)
eg_DATA = atf-run/sample/atf-run.hooks
EXTRA_DIST += $(eg_DATA)

# vim: syntax=make:noexpandtab:shiftwidth=8:softtabstop=8