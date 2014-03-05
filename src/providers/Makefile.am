include $(top_srcdir)/flymake.mk

SUBDIRS = tests

pkglibdir = $(LPF_PROVIDERS_DIR)

AM_CPPFLAGS = \
	-DG_LOG_DOMAIN="\"LibPlanFahr\"" \
	-DLIBPLANFAHR_COMPILATION \
	-DLPF_PROVIDERS_DIR="\"$(LPF_PROVIDERS_DIR)\"" \
	$(NULL)

pkglib_LTLIBRARIES = \
	libplanfahr-provider-ch-sbb.la \
	libplanfahr-provider-de-db.la \
	libplanfahr-provider-de-bvg.la \
	$(NULL)

libplanfahr_provider_ch_sbb_la_SOURCES = \
	ch-sbb.h \
	ch-sbb.c \
	hafas-bin6.h \
	hafas-bin6.c \
	$(NULL)

libplanfahr_provider_ch_sbb_la_CFLAGS = \
	$(AM_CPPFLAGS) \
	$(GIO2_CFLAGS) \
	$(GOBJECT2_CFLAGS) \
	$(GTHREAD2_CFLAGS) \
	$(LIBXML2_CFLAGS) \
        $(LIBSOUP_CFLAGS) \
	$(WARN_CFLAGS) \
	-I$(top_srcdir)/src \
	$(NULL)

libplanfahr_provider_ch_sbb_la_LDFLAGS = \
	-module \
	-avoid-version \
        $(LIBSOUP_LIBS) \
	$(NULL)

libplanfahr_provider_de_db_la_SOURCES = \
	de-db.h \
	de-db.c \
	hafas-bin6.h \
	hafas-bin6.c \
	$(NULL)

libplanfahr_provider_de_db_la_CFLAGS = \
	$(AM_CPPFLAGS) \
	$(GIO2_CFLAGS) \
	$(GOBJECT2_CFLAGS) \
	$(GTHREAD2_CFLAGS) \
	$(LIBXML2_CFLAGS) \
        $(LIBSOUP_CFLAGS) \
	$(WARN_CFLAGS) \
	-I$(top_srcdir)/src \
	$(NULL)

libplanfahr_provider_de_db_la_LDFLAGS = \
	-module \
	-avoid-version \
        $(LIBSOUP_LIBS) \
	$(NULL)

libplanfahr_provider_de_bvg_la_SOURCES = \
	de-bvg.h \
	de-bvg.c \
	hafas-bin6.h \
	hafas-bin6.c \
	$(NULL)

libplanfahr_provider_de_bvg_la_CFLAGS = \
	$(AM_CPPFLAGS) \
	$(GIO2_CFLAGS) \
	$(GOBJECT2_CFLAGS) \
	$(GTHREAD2_CFLAGS) \
	$(LIBXML2_CFLAGS) \
        $(LIBSOUP_CFLAGS) \
	$(WARN_CFLAGS) \
	-I$(top_srcdir)/src \
	$(NULL)

libplanfahr_provider_de_bvg_la_LDFLAGS = \
	-module \
	-avoid-version \
        $(LIBSOUP_LIBS) \
	$(NULL)
