#   BSD LICENSE
#
#   Copyright(c) 2010-2014 Intel Corporation. All rights reserved.
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#     * Neither the name of Intel Corporation nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, can be overriden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk

# binary name
APP = naga

# all source are stored in SRCS-y
SRCS-y := main.c  main_data.c

CFLAGS += -O3 $(USER_FLAGS)
#CFLAGS += $(WERROR_FLAGS)



#LDFLAGS += -L ../libzebra -L /mnt/hgfs/code/rose/src/dpdk-2.0.0/app/naga/cmdline -L ../vsr
CFLAGS  += -I ../cmdline  -I ../libzebra  -I ../boots -I ../include -I ../vsr -I ../pid -I ../debug
CFLAGS  += -I ../itf -I ../dmr -I ../acr -I ../adp -I ../netseg -I ../dnet -I ../urlr -I ../hijack -I ../domain
#LDFLAGS += -L ../libzebra -L /mnt/hgfs/code/rose/src/dpdk-2.0.0/app/naga/cmdline -L ../vsr
LDLIBS  +=  ../cmdline/libcmdline.a  ../libzebra/libzebra.a ../url/liburl.a
LDLIBS  +=   ../vsr/libvsr.a  ../pid/libpid.a ../itf/libitf.a   ../dmr/libdmr.a ../acr/libacr.a ../boots/libboots.a ../pub/libpub.a ../urlr/liburlr.a
LDLIBS  +=   ../adp/libadp.a ../template/libtemp.a ../debug/libdebug.a  -lpthread ../itf/libitf.a ../netseg/libnetseg.a ../dnet/libdnetseg.a ../url/liburl.a ../pcre-m/libpcre.a ../hijack/libhijack.a ../domain/libdomain.a
LDLIBS  +=   -lcrypt -lreadline -lrt -lpcap -lpthread -lz


include $(RTE_SDK)/mk/rte.app.mk
