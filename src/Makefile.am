LIBS = $(ZAKCGI_LIBS)

AM_CPPFLAGS = $(ZAKCGI_CFLAGS) \
              -DG_LOG_DOMAIN=\"ZakCGI\"

lib_LTLIBRARIES = libzakcgi.la

libzakcgi_la_SOURCES = commons.c \
                       commons.h \
                       form.c \
                       formelement.c \
                       formelementtext.c \
                       formelementsubmit.c \
                       main.c \
                       session.c \
                       tag.c \
                       url.c

libzakcgi_la_LDFLAGS = -no-undefined

libzakcgi_include_HEADERS = \
                            libzakcgi.h \
                            form.h \
                            formelement.h \
                            formelementtext.h \
                            formelementsubmit.h \
                            main.h \
                            session.h \
                            tag.h \
                            url.h

libzakcgi_includedir = $(includedir)/libzakcgi
