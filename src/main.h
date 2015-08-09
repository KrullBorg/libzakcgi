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

G_DEPRECATED_FOR (zak_cgi_main_get_env_field)
GHashTable *zak_cgi_main_get_env (ZakCgiMain *zakcgimain);
GValue *zak_cgi_main_get_env_field (ZakCgiMain *zakcgimain, const gchar *field);
void zak_cgi_main_env_foreach (ZakCgiMain *zakcgimain, GHFunc func, gpointer user_data);
G_DEPRECATED_FOR (zak_cgi_main_env_foreach)
gchar *zak_cgi_main_dump_env (ZakCgiMain *zakcgimain);

G_DEPRECATED_FOR (zak_cgi_main_get_cookie)
GHashTable *zak_cgi_main_get_cookies (ZakCgiMain *zakcgimain);
GValue *zak_cgi_main_get_cookie (ZakCgiMain *zakcgimain, const gchar *cookie);
void zak_cgi_main_cookies_foreach (ZakCgiMain *zakcgimain, GHFunc func, gpointer user_data);
G_DEPRECATED_FOR (zak_cgi_main_cookies_foreach)
gchar *zak_cgi_main_dump_cookies (ZakCgiMain *zakcgimain);

gchar *zak_cgi_main_set_cookie (const gchar *name,
                                const gchar *value,
                                GDateTime *expires,
                                const gchar *domain,
                                const gchar *path,
                                gboolean secure,
                                gboolean http_only);

G_DEPRECATED_FOR (zak_cgi_main_get_parameter)
GHashTable *zak_cgi_main_get_parameters (ZakCgiMain *zakcgimain, const gchar *query_string);
gchar *zak_cgi_main_get_parameter (ZakCgiMain *zakcgimain, const gchar *param);
void zak_cgi_main_parameters_foreach (ZakCgiMain *zakcgimain, GHFunc func, gpointer user_data);


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

G_DEPRECATED_FOR (zak_cgi_main_get_stdin_field)
GHashTable *zak_cgi_main_parse_stdin (const gchar *buf, const gchar *boundary);
GValue *zak_cgi_main_get_stdin_field (ZakCgiMain *zakcgimain, const gchar *field);
void zak_cgi_main_stdin_foreach (ZakCgiMain *zakcgimain, GHFunc func, gpointer user_data);

void zak_cgi_main_redirect (ZakCgiMain *zakcgimain, const gchar *url);

gboolean zak_cgi_main_is_request_method (ZakCgiMain *zakcgimain, const gchar *method);

gboolean zak_cgi_main_is_get (ZakCgiMain *zakcgimain);
gboolean zak_cgi_main_is_post (ZakCgiMain *zakcgimain);


G_END_DECLS


#endif /* __ZAK_CGI_MAIN_H__ */
