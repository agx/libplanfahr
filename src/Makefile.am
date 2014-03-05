include $(top_srcdir)/flymake.mk

SUBDIRS = providers

EXTRA_DIST = libplanfahr.sym

lib_LTLIBRARIES = libplanfahr-0.0.la

PLANFAHR_HEADER_FILES = \
	libplanfahr.h \
	lpf-manager.h \
	lpf-priv.h \
	lpf-provider.h \
	lpf-loc.h \
	lpf-stop.h \
	lpf-trip.h \
	lpf-trip-part.h \
	$(NULL)

PLANFAHR_SOURCE_FILES = \
	lpf-manager.c \
	lpf-provider.c \
	lpf-loc.c \
	lpf-stop.c \
	lpf-trip.c \
	lpf-trip-part.c \
	$(NULL)

libplanfahr_0_0_ladir = $(includedir)/libplanfahr/
libplanfahr_0_0_la_HEADERS = \
	$(PLANFAHR_HEADER_FILES) \
	$(NULL)

libplanfahr_0_0_la_SOURCES = \
	$(PLANFAHR_SOURCE_FILES) \
	$(NULL)

libplanfahr_0_0_la_CFLAGS = \
	-DG_LOG_DOMAIN="\"LibPlanFahr\"" \
        -DLPF_PROVIDERS_DIR="\"$(LPF_PROVIDERS_DIR)\"" \
	-I$(top_srcdir) \
	$(GIO2_CFLAGS) \
	$(GOBJECT2_CFLAGS) \
	$(GTHREAD2_CFLAGS) \
	$(WARN_CFLAGS) \
	$(NULL)

libplanfahr_0_0_la_LIBADD = \
	$(GIO2_LIBS) \
	$(GOBJECT2_LIBS) \
	$(GTHREAD2_LIBS) \
	$(NULL)

libplanfahr_0_0_la_DEPENDENCIES = \
	libplanfahr.sym \
	$(NULL)

libplanfahr_0_0_la_LDFLAGS = \
	-Wl,--version-script=$(srcdir)/libplanfahr.sym \
	-version-info $(LIBPLANFAHR_VERSION_INFO)

if HAVE_INTROSPECTION

INTROSPECTION_GIRS = Lpf-0.0.gir
INTROSPECTION_SCANNER_ARGS = --warn-all
INTROSPECTION_COMPILER_ARGS =

Lpf-0.0.gir: libplanfahr-0.0.la
Lpf_0_0_gir_INCLUDES = GObject-2.0 Gio-2.0
Lpf_0_0_gir_CFLAGS = $(libplanfahr_0_0_la_CFLAGS)
Lpf_0_0_gir_LIBS = $(builddir)/libplanfahr-0.0.la
Lpf_0_0_gir_EXPORT_PACKAGES = libplanfahr-0.0
Lpf_0_0_gir_SCANNERFLAGS = \
	--c-include="libplanfahr/libplanfahr.h" \
	--namespace=Lpf \
	--symbol-prefix=lpf \
	--pkg=gthread-2.0 \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	$(NULL)
Lpf_0_0_gir_FILES = \
	$(srcdir)/libplanfahr.h \
	$(PLANFAHR_SOURCE_FILES:%=$(srcdir)/%) \
	$(PLANFAHR_HEADER_FILES:%=$(srcdir)/%) \
	$(NULL)

girdir = $(datadir)/gir-1.0
nodist_gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
nodist_typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES = $(gir_DATA) $(typelibs_DATA)

endif # HAVE_INTROSPECTION

-include $(INTROSPECTION_MAKEFILE)
