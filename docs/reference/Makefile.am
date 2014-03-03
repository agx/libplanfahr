## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = 1.6

# The name of the module
DOC_MODULE=libplanfahr-0.0

# The top-level SGML file.
DOC_MAIN_SGML_FILE=$(DOC_MODULE)-docs.sgml

# The directory containing the source code. Relative to $(srcdir).
# gtk-doc will search all .c & .h files beneath here for inline comments
# documenting functions and macros.
DOC_SOURCE_DIR=../../src

# Extra options to supply to gtkdoc-scan.
SCAN_OPTIONS='--rebuild-types'

# Extra options to supply to gtkdoc-scangobj.
SCANGOBJ_OPTIONS=

# Extra options to supply to gtkdoc-mkdb.
MKDB_OPTIONS=--sgml-mode --output-format=xml

# Extra options to supply to gtkdoc-fixref.
FIXXREF_OPTIONS=

# Used for dependencies.
HFILE_GLOB=
CFILE_GLOB=

# Header files to ignore when scanning.
IGNORE_HFILES = \
	planfahr.h 	\
	de-bvg.h    	\
	de-db.h    	\
	hafas-bin6.h 	\
	$(NULL)

# Images to copy into HTML directory.
HTML_IMAGES = 

# Extra XML files that are included by $(DOC_MAIN_SGML_FILE).
content_files =				\
	build-howto.xml			\
	$(NULL)

# Other files to distribute.
extra_files =


# CFLAGS and LDFLAGS for compiling scan program. Only needed
# if $(DOC_MODULE).types is non-empty.
GTKDOC_CFLAGS =				\
	-I$(top_srcdir)			\
	-I$(top_builddir)		\
	$(GLIB_CFLAGS)			\
	$(XML_CFLAGS)			\
	$(NULL)

GTKDOC_LIBS =					\
	$(top_builddir)/src/libplanfahr-0.0.la	\
	$(GLIB_LIBS)

# include common portion ...
include $(top_srcdir)/gtk-doc.make

# kludges
$(srcdir)/tmpl/*.sgml:

clean: clean-am
	rm -rf tmpl
