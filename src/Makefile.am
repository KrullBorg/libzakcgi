if PLATFORM_WIN32
libmodulesext = -0.dll
libmodulesdir = $(bindir)
else
libmodulesext = .so
libmodulesdir = $(libdir)
endif

libzakformmodulesdir = `pkg-config --variable=modulesdir libzakform`

LIBS = $(ZAKCGI_LIBS)

AM_CPPFLAGS = $(ZAKCGI_CFLAGS) \
              -DG_LOG_DOMAIN=\"ZakCGI\"

lib_LTLIBRARIES = libzakcgi.la

libzakcgi_la_SOURCES = commons.c \
                       commons.h \
                       form.c \
                       formelement.c \
                       formelementcheck.c \
                       formelementhidden.c \
                       formelementpassword.c \
                       formelementselect.c \
                       formelementstring.c \
                       formelementtext.c \
                       formelementtextarea.c \
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
                            formelementcheck.h \
                            formelementhidden.h \
                            formelementpassword.h \
                            formelementselect.h \
                            formelementstring.h \
                            formelementtext.h \
                            formelementtextarea.h \
                            formelementsubmit.h \
                            main.h \
                            session.h \
                            tag.h \
                            url.h

libzakcgi_includedir = $(includedir)/libzakcgi

install-exec-hook:
	cd $(libzakformmodulesdir) && \
	ln -s -f $(libmodulesdir)/libzakcgi$(libmodulesext) .

uninstall-hook:
	rm -f $(libzakformmodulesdir)/libzakcgi$(libmodulesext)
