include $(top_srcdir)/flymake.mk

check_PROGRAMS = de-db hafas-bin6

AM_CFLAGS = \
	$(GLIB2_CFLAGS) \
	$(GIO2_CFLAGS) \
	$(GOBJECT2_CFLAGS) \
	$(GTHREAD2_CFLAGS) \
	$(LIBXML2_CFLAGS) \
	$(WARN_CFLAGS) \
	-I$(top_srcdir)/src \
	-DLPF_TEST_SRCDIR=\""$(abs_srcdir)"\" \
	$(NULL)

LDADD = \
	$(top_builddir)/src/libplanfahr-0.0.la \
	$(GLIB2_LIBS) \
	$(GIO2_LIBS) \
	$(GOBJECT2_LIBS) \
	$(GTHREAD2_LIBS) \
	$(NULL)

de_db_SOURCES = \
	de-db.c \
	$(NULL)

de_db_CFLAGS = \
	$(AM_CFLAGS) \
	$(LIBSOUP_CFLAGS) \
	$(LIBXML2_CFLAGS) \
	$(NULL)
de_db_LDADD = \
	../libplanfahr-provider-de-db.la \
	$(LDADD) \
	$(LIBSOUP_LIBS) \
	$(LIBXML2_LIBS) \
	$(NULL)

hafas_bin6_SOURCES = \
        hafas-bin6.c           \
	../hafas-bin6-format.h \
	$(NULL)
hafas_bin6_CFLAGS = \
	$(AM_CFLAGS) \
	$(NULL)
hafas_bin6_LDADD = \
	$(LDADD) \
	$(NULL)


uninstalled_testdir = tests/
dist_uninstalled_test_DATA = \
	hafas-bin-6-station-query-1.bin \
	$(NULL)

TESTS = $(check_PROGRAMS)
