include $(top_srcdir)/flymake.mk
include $(top_srcdir)/glib-tap.mk

test_programs = hafas-bin6 hafas-bin6-format

AM_CPPFLAGS = \
	-DLIBPLANFAHR_COMPILATION \
	-DLPF_TEST_SRCDIR=\""$(abs_srcdir)"\" \
	$(GLIB2_CFLAGS) \
	$(GIO2_CFLAGS) \
	$(GOBJECT2_CFLAGS) \
	$(GTHREAD2_CFLAGS) \
	$(LIBXML2_CFLAGS) \
	$(WARN_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libplanfahr \
	$(NULL)

LDADD = \
	$(top_builddir)/libplanfahr/libplanfahr-0.0.la \
	$(GLIB2_LIBS) \
	$(GIO2_LIBS) \
	$(GOBJECT2_LIBS) \
	$(GTHREAD2_LIBS) \
	$(NULL)

hafas_bin6_SOURCES = \
	hafas-bin6.c \
	$(NULL)

hafas_bin6_CFLAGS = \
	$(AM_CPPFLAGS) \
	$(LIBSOUP_CFLAGS) \
	$(LIBXML2_CFLAGS) \
	$(NULL)
hafas_bin6_LDADD = \
	../libplanfahr-provider-de-db.la \
	$(LDADD) \
	$(LIBSOUP_LIBS) \
	$(LIBXML2_LIBS) \
	$(NULL)

hafas_bin6_format_SOURCES = \
        hafas-bin6-format.c    \
	../hafas-bin6-format.h \
	$(NULL)
hafas_bin6_format_CFLAGS = \
	$(AM_CPPFLAGS) \
	$(NULL)
hafas_bin6_format_LDADD = \
	$(LDADD) \
	$(NULL)


dist_test_data = \
	hafas-bin-6-station-query-1.bin \
	$(NULL)

