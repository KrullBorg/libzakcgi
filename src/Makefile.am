LIBS = $(ZAKCGI_LIBS)

AM_CPPFLAGS = $(ZAKCGI_CFLAGS) \
              -DG_LOG_DOMAIN=\"ZakCGI\"

lib_LTLIBRARIES = libzakcgi.la

libzakcgi_la_SOURCES = commons.c \
                       main.c \
                       session.c \
                       tag.c \
                       url.c

libzakcgi_la_LDFLAGS = -no-undefined

libzakcgi_include_HEADERS = libzakcgi.h \
                            commons.h \
                            main.h \
                            session.h \
                            tag.h \
                            url.h

libzakcgi_includedir = $(includedir)/libzakcgi

CLEANFILES =

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
introspection_sources = $(libzakcgi_la_SOURCES) $(libzakcgi_include_HEADERS)

ZakCgi-1.0.gir: libzakcgi.la
ZakCgi_1_0_gir_INCLUDES = GObject-2.0 GLib-2.0 Gio-2.0
ZakCgi_1_0_gir_CFLAGS = $(AM_CPPFLAGS)
ZakCgi_1_0_gir_LIBS = libzakcgi.la
ZakCgi_1_0_gir_FILES = $(introspection_sources)
INTROSPECTION_GIRS += ZakCgi-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif
