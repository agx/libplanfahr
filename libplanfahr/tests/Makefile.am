include $(top_srcdir)/flymake.mk
include $(top_srcdir)/glib-tap.mk

test_programs = lpf-manager

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

lpf_manager_SOURCES = \
	lpf-manager.c \
	$(NULL)

lpf_manager_CFLAGS = \
	$(AM_CPPFLAGS) \
	$(LIBSOUP_CFLAGS) \
	$(LIBXML2_CFLAGS) \
	$(NULL)
lpf_manager_LDADD = \
	../providers/libplanfahr-provider-test-test.la \
	$(LDADD) \
	$(LIBSOUP_LIBS) \
	$(LIBXML2_LIBS) \
	$(NULL)

dist_test_data = \
	$(NULL)

