LIBS = $(ZAKCGI_LIBS)

AM_CPPFLAGS = $(ZAKCGI_CFLAGS) \
              -DG_LOG_DOMAIN=\"ZakCGI\"

lib_LTLIBRARIES = libzakcgi.la

libzakcgi_la_SOURCES = zakcgimain.c

libzakcgi_la_LDFLAGS = -no-undefined

libzakcgi_include_HEADERS = libzakcgi.h \
                            zakcgimain.h

libzakcgi_includedir = $(includedir)/libzakcgi
