/*
 * Copyright (C) 2015 Andrea Zagli <azagli@libero.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __ZAK_CGI_MAIN_H__
#define __ZAK_CGI_MAIN_H__


#include <glib-object.h>


G_BEGIN_DECLS


#define ZAK_CGI_TYPE_MAIN                 (zak_cgi_main_get_type ())
#define ZAK_CGI_MAIN(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZAK_CGI_TYPE_MAIN, ZakCgiMain))
#define ZAK_CGI_MAIN_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), ZAK_CGI_TYPE_MAIN, ZakCgiMainClass))
#define ZAK_CGI_IS_MAIN(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZAK_CGI_TYPE_MAIN))
#define ZAK_CGI_IS_MAIN_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), ZAK_CGI_TYPE_MAIN))
#define ZAK_CGI_MAIN_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), ZAK_CGI_TYPE_MAIN, ZakCgiMainClass))

typedef struct _ZakCgiMain ZakCgiMain;
typedef struct _ZakCgiMainClass ZakCgiMainClass;

struct _ZakCgiMain
	{
		GObject parent_instance;
	};

struct _ZakCgiMainClass
	{
		GObjectClass parent_class;
	};

GType zak_cgi_main_get_type (void);


#define ZAK_CGI_STANDARD_HEADER_HTML "Content-Type: text/html; charset=UTF-8"


ZakCgiMain *zak_cgi_main_new (void);

void zak_cgi_main_out (const gchar *header, const gchar *body);

GHashTable *zak_cgi_main_get_env (ZakCgiMain *zakcgimain);
gchar *zak_cgi_main_dump_env (ZakCgiMain *zakcgimain);

GHashTable *zak_cgi_main_get_cookies (ZakCgiMain *zakcgimain);
gchar *zak_cgi_main_dump_cookies (ZakCgiMain *zakcgimain);

gchar *zak_cgi_main_set_cookie (const gchar *name,
                                const gchar *value,
                                GDateTime *expires,
                                const gchar *domain,
                                const gchar *path,
                                gboolean secure,
                                gboolean http_only);

GHashTable *zak_cgi_main_get_parameters (ZakCgiMain *zakcgimain, const gchar *query_string);


typedef struct _ZakCgiFile ZakCgiFile;

struct _ZakCgiFile
{
	gchar *name;
	gchar *content;
    guint size;
};

#define ZAK_CGI_TYPE_FILE (zak_cgi_file_get_type ())

GType zak_cgi_file_get_type ();


gchar *zak_cgi_main_get_stdin (ZakCgiMain *zakcgimain);

GHashTable *zak_cgi_main_parse_stdin (const gchar *buf, const gchar *boundary);

void zak_cgi_main_redirect (const gchar *url);


G_END_DECLS


#endif /* __ZAK_CGI_MAIN_H__ */
