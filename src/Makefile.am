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
	$(GTHREAD2_LIBS) 
	$(NULL)

libplanfahr_0_0_la_DEPENDENCIES = \
	libplanfahr.sym \
	$(NULL)

libplanfahr_0_0_la_LDFLAGS = \
	-Wl,--version-script=$(srcdir)/libplanfahr.sym \
	-version-info $(LIBPLANFAHR_VERSION_INFO)

if WITH_GOBJECT_INTROSPECTION

Lpf-0.0.gir: libplanfahr-0.0.la $(G_IR_SCANNER) Makefile.am
	$(AM_V_GEN)$(G_IR_SCANNER) \
                --quiet \
                --warn-all \
                --namespace Lpf \
                --nsversion 0.0 \
                --include GObject-2.0 \
                --include Gio-2.0 \
                --symbol-prefix=lpf \
                --library=$(builddir)/libplanfahr-0.0.la \
                --output $@ \
                -I$(top_builddir) \
                -I$(top_srcdir) \
                --verbose \
                --c-include="libplanfahr/libplanfahr.h" \
                --pkg=gthread-2.0 \
                --pkg-export=libplanfahr-0.0 \
                $(srcdir)/libplanfahr.h \
                $(PLANFAHR_SOURCE_FILES:%=$(srcdir)/%) \
                $(PLANFAHR_HEADER_FILES:%=$(srcdir)/%)

girdir = $(datadir)/gir-1.0
gir_DATA = Lpf-0.0.gir

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = Lpf-0.0.typelib

%.typelib: %.gir
	$(AM_V_GEN)$(G_IR_COMPILER) \
                --includedir=$(builddir) \
                --includedir=$(girdir) \
                -o $@ $<

CLEANFILES = $(gir_DATA) $(typelibs_DATA)

endif # WITH_GOBJECT_INTROSPECTION
