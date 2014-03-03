AC_INIT(libplanfahr, 0.0.0)
AC_CONFIG_SRCDIR(src/libplanfahr.h)
AC_CONFIG_AUX_DIR([build-aux])
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])
dnl Make automake keep quiet about wildcards & other GNUmake-isms
AM_INIT_AUTOMAKE([1.11.1 subdir-objects no-dist-gzip dist-xz -Wno-portability])
AC_CANONICAL_HOST

AM_SILENT_RULES([yes])

GLIB2_REQUIRED=2.22.0
GOBJECT2_REQUIRED=2.10.0
GIO_REQUIRED=2.10.0
GOBJECT_INTROSPECTION_REQUIRED=1.39.90
LIBXML2_REQUIRED=2.0.0
LIBSOUP_REQUIRED=2.42

LIBPLANFAHR_MAJOR_VERSION=`echo $VERSION | awk -F. '{print $1}'`
LIBPLANFAHR_MINOR_VERSION=`echo $VERSION | awk -F. '{print $2}'`
LIBPLANFAHR_MICRO_VERSION=`echo $VERSION | awk -F. '{print $3}'`
LIBPLANFAHR_VERSION=$LIBPLANFAHR_MAJOR_VERSION.$LIBPLANFAHR_MINOR_VERSION.$LIBPLANFAHR_MICRO_VERSION$LIBPLANFAHR_MICRO_VERSION_SUFFIX
LIBPLANFAHR_VERSION_INFO=`expr $LIBPLANFAHR_MAJOR_VERSION + $LIBPLANFAHR_MINOR_VERSION`:$LIBPLANFAHR_MICRO_VERSION:$LIBPLANFAHR_MINOR_VERSION
LIBPLANFAHR_VERSION_NUMBER=`expr $LIBPLANFAHR_MAJOR_VERSION \* 1000000 + $LIBPLANFAHR_MINOR_VERSION \* 1000 + $LIBPLANFAHR_MICRO_VERSION`

AC_SUBST([LIBPLANFAHR_MAJOR_VERSION])
AC_SUBST([LIBPLANFAHR_MINOR_VERSION])
AC_SUBST([LIBPLANFAHR_MICRO_VERSION])
AC_SUBST([LIBPLANFAHR_VERSION])
AC_SUBST([LIBPLANFAHR_VERSION_INFO])
AC_SUBST([LIBPLANFAHR_VERSION_NUMBER])

AC_PROG_CC
AM_PROG_CC_C_O

AC_PROG_LIBTOOL

LIBPLANFAHR_COMPILE_WARNINGS

AC_DEFINE([_GNU_SOURCE], [], [Enable GNU extensions])

PKG_CHECK_MODULES(GLIB2, glib-2.0 >= $GLIB2_REQUIRED)
PKG_CHECK_MODULES(GTHREAD2, gthread-2.0 >= $GLIB2_REQUIRED)
PKG_CHECK_MODULES(GOBJECT2, gobject-2.0 >= $GLIB2_REQUIRED)
PKG_CHECK_MODULES(GIO2, gio-2.0 >= $GLIB2_REQUIRED)
PKG_CHECK_MODULES(LIBXML2, libxml-2.0 >= $LIBXML2_REQUIRED)
PKG_CHECK_MODULES(LIBSOUP, libsoup-2.4 >= $LIBSOUP_REQUIRED)

AC_ARG_ENABLE([introspection],
        AS_HELP_STRING([--enable-introspection], [enable GObject introspection]),
        [], [enable_introspection=check])

if test "x$enable_introspection" != "xno" ; then
        PKG_CHECK_MODULES([GOBJECT_INTROSPECTION],
                          [gobject-introspection-1.0 >= $GOBJECT_INTROSPECTION_REQUIRED],
                          [enable_introspection=yes],
                          [
                             if test "x$enable_introspection" = "xcheck"; then
                               enable_introspection=no
                             else
                               AC_MSG_ERROR([gobject-introspection is not available])
                             fi
                          ])
        if test "x$enable_introspection" = "xyes" ; then
          AC_DEFINE([WITH_GOBJECT_INTROSPECTION], [1], [enable GObject introspection support])
          AC_SUBST(GOBJECT_INTROSPECTION_CFLAGS)
          AC_SUBST(GOBJECT_INTROSPECTION_LIBS)
          AC_SUBST([G_IR_SCANNER], [$($PKG_CONFIG --variable=g_ir_scanner gobject-introspection-1.0)])
          AC_SUBST([G_IR_COMPILER], [$($PKG_CONFIG --variable=g_ir_compiler gobject-introspection-1.0)])
        fi
fi
AM_CONDITIONAL([WITH_GOBJECT_INTROSPECTION], [test "x$enable_introspection" = "xyes"])


GTK_DOC_CHECK([1.10])

dnl --enable-debug=(yes|no)
AC_ARG_ENABLE(debug,
              AS_HELP_STRING([--enable-debug=no/yes],[enable debugging output]),[],[enable_debug=no])
if test x"$enable_debug" = x"yes"; then
   AC_DEFINE(ENABLE_DEBUG, 1, [whether debugging is enabled])
fi

# Setup GLIB_MKENUMS to use glib-mkenums even if GLib is uninstalled.
GLIB_MKENUMS=`$PKG_CONFIG --variable=glib_mkenums glib-2.0`
AC_SUBST(GLIB_MKENUMS)

LPF_PROVIDERS_DIR="$libdir/libplanfahr/providers"
AC_SUBST(LPF_PROVIDERS_DIR)

AC_CONFIG_FILES([run],
                [chmod +x,-w run])
AC_CONFIG_FILES(Makefile
                src/Makefile
                src/providers/Makefile
                src/providers/tests/Makefile
                data/Makefile
                data/libplanfahr-0.0.pc
                docs/Makefile
                docs/reference/Makefile
)
AC_OUTPUT

