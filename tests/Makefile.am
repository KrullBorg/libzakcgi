AM_CPPFLAGS = $(WARN_CFLAGS) \
              $(DISABLE_DEPRECATED_CFLAGS) \
              $(ZAKCGI_CFLAGS) \
              -I$(top_srcdir)/src

LIBS = $(ZAKCGI_LIBS) \
       -export-dynamic

LDADD = $(top_builddir)/src/libzakcgi.la

noinst_PROGRAMS = \
                    env \
                    querystring
