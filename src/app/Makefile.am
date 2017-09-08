
bin_PROGRAMS = abcip

abcip_SOURCES = \
main.cc \
pcap_writer.cc pcap_writer.h

abcip_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/src/common -I$(top_srcdir)/src/protos

abcip_LDADD = \
$(top_builddir)/src/protos/libproto.la \
$(top_builddir)/src/common/libcommon.la

if BUILD_DAQCAP_WRITER
abcip_SOURCES += daqcap_writer.cc daqcap_writer.h
abcip_CPPFLAGS += $(DAQCAP_CPPFLAGS)
abcip_LDFLAGS = $(AM_LDFLAGS) $(DAQCAP_LDFLAGS)
abcip_LDADD   += -ldaqcap
endif
