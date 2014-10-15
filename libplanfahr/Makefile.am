include $(top_srcdir)/flymake.mk

SUBDIRS = providers tests

EXTRA_DIST = libplanfahr.sym \
	lpf-enumtypes.c \
	lpf-enumtypes.h \
	$(NULL)

BUILT_SOURCES = \
	lpf-enumtypes.c \
	lpf-enumtypes.h \
	$(NULL)

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

PLANFAHR_INCLUDE_HEADER_FILES = \
	$(PLANFAHR_HEADER_FILES) \
	lpf-enumtypes.h \
	$(NULL)

PLANFAHR_SOURCE_FILES = \
	lpf-enumtypes.c \
	lpf-manager.c \
	lpf-provider.c \
	lpf-loc.c \
	lpf-stop.c \
	lpf-trip.c \
	lpf-trip-part.c \
	$(NULL)

lpf-enumtypes.h: $(PLANFAHR_HEADER_FILES) lpf-enumtypes.h.template
	$(AM_V_GEN) $(GLIB_MKENUMS) --template $(filter %.template,$^) $(filter-out %.template,$^) > \
	    lpf-enumtypes.h.tmp && mv lpf-enumtypes.h.tmp lpf-enumtypes.h

lpf-enumtypes.c: $(PLANFAHR_HEADER_FILES) lpf-enumtypes.c.template
	$(AM_V_GEN) $(GLIB_MKENUMS) --template $(filter %.template,$^) $(filter-out %.template,$^) > \
	    lpf-enumtypes.c.tmp && mv lpf-enumtypes.c.tmp lpf-enumtypes.c

libplanfahr_0_0_ladir = $(includedir)/libplanfahr-0.0/libplanfahr
libplanfahr_0_0_la_HEADERS = \
	$(PLANFAHR_INCLUDE_HEADER_FILES) \
	$(NULL)

libplanfahr_0_0_la_SOURCES = \
	$(PLANFAHR_SOURCE_FILES) \
	$(NULL)

libplanfahr_0_0_la_CFLAGS = \
	-DG_LOG_DOMAIN="\"LibPlanFahr\"" \
	-DLIBPLANFAHR_COMPILATION \
	-DLPF_PROVIDERS_DIR="\"$(LPF_PROVIDERS_DIR)\"" \
	-I$(top_srcdir) \
	$(GIO2_CFLAGS) \
	$(GOBJECT2_CFLAGS) \
	$(GTHREAD2_CFLAGS) \
	$(GMODULE_CFLAGS) \
	$(WARN_CFLAGS) \
	$(NULL)

libplanfahr_0_0_la_LIBADD = \
	$(GIO2_LIBS) \
	$(GOBJECT2_LIBS) \
	$(GTHREAD2_LIBS) \
	$(GMODULE_LIBS) \
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
	--namespace=Lpf \
	--symbol-prefix=lpf \
	--pkg=gthread-2.0 \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	$(NULL)
Lpf_0_0_gir_FILES = \
	$(PLANFAHR_SOURCE_FILES:%=$(srcdir)/%) \
	$(PLANFAHR_INCLUDE_HEADER_FILES:%=$(srcdir)/%) \
	$(NULL)

girdir = $(datadir)/gir-1.0
nodist_gir_DATA = $(INTROSPECTION_GIRS)
typelibdir = $(libdir)/girepository-1.0
nodist_typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES = \
	$(nodist_gir_DATA) \
	$(nodist_typelib_DATA) \
	lpf-enumtypes.c \
	lpf-enumtypes.h \
	$(NULL)

endif # HAVE_INTROSPECTION

-include $(INTROSPECTION_MAKEFILE)
