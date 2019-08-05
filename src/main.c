/*
 * Copyright (C) 2015-2019 Andrea Zagli <azagli@libero.it>
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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <glib/gprintf.h>
#include <glib/gstdio.h>
#include <gio/gunixinputstream.h>

#include "main.h"

static void zak_cgi_main_class_init (ZakCgiMainClass *class);
static void zak_cgi_main_init (ZakCgiMain *zak_cgi_main);

static void zak_cgi_main_set_property (GObject *object,
                                       guint property_id,
                                       const GValue *value,
                                       GParamSpec *pspec);
static void zak_cgi_main_get_property (GObject *object,
                                       guint property_id,
                                       GValue *value,
                                       GParamSpec *pspec);

static void zak_cgi_main_dispose (GObject *gobject);
static void zak_cgi_main_finalize (GObject *gobject);

#define ZAK_CGI_MAIN_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_CGI_TYPE_MAIN, ZakCgiMainPrivate))

typedef struct _ZakCgiMainPrivate ZakCgiMainPrivate;
struct _ZakCgiMainPrivate
	{
		GHashTable *ht_env;
		GHashTable *ht_cookies;
		GHashTable *ht_parameters;
		gchar *stdin;
	};

G_DEFINE_TYPE (ZakCgiMain, zak_cgi_main, G_TYPE_OBJECT)

static void
zak_cgi_main_class_init (ZakCgiMainClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = zak_cgi_main_set_property;
	object_class->get_property = zak_cgi_main_get_property;
	object_class->dispose = zak_cgi_main_dispose;
	object_class->finalize = zak_cgi_main_finalize;

	g_type_class_add_private (object_class, sizeof (ZakCgiMainPrivate));
}

static void
zak_cgi_main_init (ZakCgiMain *zak_cgi_main)
{
	ZakCgiMainPrivate *priv = ZAK_CGI_MAIN_GET_PRIVATE (zak_cgi_main);

	priv->ht_env = NULL;
	priv->ht_cookies = NULL;
	priv->ht_parameters = NULL;
	priv->stdin = NULL;
}

/**
 * zak_cgi_main_new:
 *
 * Returns: the newly created #ZakCgiMain object.
 */
ZakCgiMain
*zak_cgi_main_new (void)
{
	ZakCgiMain *zak_cgi_main;
	ZakCgiMainPrivate *priv;

	zak_cgi_main = ZAK_CGI_MAIN (g_object_new (zak_cgi_main_get_type (), NULL));

	priv = ZAK_CGI_MAIN_GET_PRIVATE (zak_cgi_main);

	return zak_cgi_main;
}

/**
 * zak_cgi_main_out:
 * header:
 * body:
 *
 */
void
zak_cgi_main_out (const gchar *header, const gchar *body)
{
	gchar *_header;

	if (header == NULL)
		{
			_header = g_strdup (ZAK_CGI_STANDARD_HEADER_HTML);
		}
	else
		{
			_header = g_strdup (header);
		}

	g_printf ("%s%c%c\n%s\n",
	          _header,
	          13, 10,
	          body);

	g_free (_header);
}

/**
 * zak_cgi_main_out_not_null_term:
 * header:
 * body:
 * length:
 *
 */
void
zak_cgi_main_out_not_null_term (const gchar *header, const gchar *body, guint length)
{
	guint i;
	gchar *_header;

	if (header == NULL)
		{
			_header = g_strdup (ZAK_CGI_STANDARD_HEADER_HTML);
		}
	else
		{
			_header = g_strdup (header);
		}

	g_printf ("%s%c%c\n",
	          _header,
	          13, 10);

	for (i = 0; i < length; i++)
		{
			g_printf ("%c", body[i]);
		}
	g_printf ("\n");

	g_free (_header);
}

static GHashTable
*_zak_cgi_main_get_env (ZakCgiMain *zakcgimain)
{
	ZakCgiMainPrivate *priv;
	GHashTable *ht;

	gchar **environ;
	guint l;
	guint i;
	gchar **envs;

	GValue *gval;

	if (zakcgimain != NULL)
		{
			priv = ZAK_CGI_MAIN_GET_PRIVATE (zakcgimain);
			if (priv->ht_env != NULL)
				{
					return g_hash_table_ref (priv->ht_env);
				}
		}

	ht = g_hash_table_new (g_str_hash, g_str_equal);
	if (zakcgimain != NULL)
		{
			priv->ht_env = g_hash_table_ref (ht);
		}

	environ = g_get_environ ();
	l = g_strv_length (environ);
	for (i = 0; i < l; i++)
		{
			envs = g_strsplit (environ[i], "=", 2);

			gval = (GValue *)g_new0 (GValue, 1);
			g_value_init (gval, G_TYPE_STRING);
			g_value_take_string (gval, g_strdup (envs[1]));
			g_hash_table_replace (ht, g_strdup (envs[0]), gval);
			g_strfreev (envs);
		}

	return ht;
}

/**
 * zak_cgi_main_get_env:
 * @zakcgimain:
 *
 * Returns: a #GHashTable with all the environment variables as #GValue.
 */
G_DEPRECATED_FOR (zak_cgi_main_get_env_field)
GHashTable
*zak_cgi_main_get_env (ZakCgiMain *zakcgimain)
{
	return _zak_cgi_main_get_env (zakcgimain);
}

/**
 * zak_cgi_main_get_env_field:
 * @zakcgimain:
 * @field:
 *
 * Returns:
 */
GValue
*zak_cgi_main_get_env_field (ZakCgiMain *zakcgimain, const gchar *field)
{
	ZakCgiMainPrivate *priv;

	GHashTable *ht;

	GValue *ret;

	g_return_val_if_fail (ZAK_CGI_IS_MAIN (zakcgimain), NULL);

	ht = _zak_cgi_main_get_env (zakcgimain);

	ret = g_hash_table_lookup (ht, field);

	return ret;
}

/**
 * zak_cgi_main_env_foreach:
 * @zakcgimain:
 * @func:
 * @user_data:
 *
 */
void
zak_cgi_main_env_foreach (ZakCgiMain *zakcgimain, GHFunc func, gpointer user_data)
{
	ZakCgiMainPrivate *priv;

	GHashTable *ht;

	g_return_if_fail (ZAK_CGI_IS_MAIN (zakcgimain));
	g_return_if_fail (func != NULL);

	ht = _zak_cgi_main_get_env (zakcgimain);

	g_hash_table_foreach (ht, func, user_data);
}

/**
 * zak_cgi_main_dump_env:
 * @zakcgimain:
 *
 * Returns: an html table with each environment variables.
 */
G_DEPRECATED_FOR (zak_cgi_main_env_foreach)
gchar
*zak_cgi_main_dump_env (ZakCgiMain *zakcgimain)
{
	GHashTable *ht_env;
	GHashTableIter iter;

	GString *str;
	gchar *ret;

	gpointer key;
	gpointer value;

	ht_env = _zak_cgi_main_get_env (zakcgimain);

	str = g_string_new ("");

	if (g_hash_table_size (ht_env) > 0)
		{
			g_string_append_printf (str, "<table>\n");

			g_hash_table_iter_init (&iter, ht_env);
			while (g_hash_table_iter_next (&iter, &key, &value))
				{
					g_string_append_printf (str, "<tr><td>%s</td><td>%s</td></tr>\n",
					                        (gchar *)key, g_value_get_string ((GValue *)value));
				}

			g_string_append_printf (str, "</table>\n");
		}

	ret = g_strdup (str->str);
	g_string_free (str, TRUE);

	return ret;
}

static GHashTable
*_zak_cgi_main_get_cookies (ZakCgiMain *zakcgimain)
{
	ZakCgiMainPrivate *priv;
	GHashTable *ht;

	guint l;
	guint i;
	const gchar *cookies;
	gchar **strv_cookies;
	gchar **parts;
	GValue *gval;

	if (zakcgimain != NULL)
		{
			priv = ZAK_CGI_MAIN_GET_PRIVATE (zakcgimain);
			if (priv->ht_cookies != NULL)
				{
					return priv->ht_cookies;
				}
		}
	ht = g_hash_table_new (g_str_hash, g_str_equal);
	if (zakcgimain != NULL)
		{
			priv->ht_cookies = ht;
		}

	gval = zak_cgi_main_get_env_field (zakcgimain, "HTTP_COOKIE");
	if (gval != NULL)
		{
			cookies = g_value_get_string (gval);
			if (cookies != NULL)
				{
					strv_cookies = g_strsplit (cookies, ";", -1);
					l = g_strv_length (strv_cookies);
					for (i = 0; i < l; i++)
						{
							parts = g_strsplit (strv_cookies[i], "=", 2);
							gval = (GValue *)g_new0 (GValue, 1);
							g_value_init (gval, G_TYPE_STRING);
							g_value_take_string (gval, g_strdup (parts[1]));
							g_hash_table_replace (ht, g_strdup (parts[0]), gval);
							g_strfreev (parts);
						}
				}
		}

	return ht;
}

/**
 * zak_cgi_main_get_cookies:
 * @zakcgimain:
 *
 * Returns: a #GHashTable with all the cookies.
 */
G_DEPRECATED_FOR (zak_cgi_main_get_cookie)
GHashTable
*zak_cgi_main_get_cookies (ZakCgiMain *zakcgimain)
{
	return _zak_cgi_main_get_cookies (zakcgimain);
}

/**
 * zak_cgi_main_get_cookie:
 * @zakcgimain:
 * @cookie:
 *
 * Returns:
 */
GValue
*zak_cgi_main_get_cookie (ZakCgiMain *zakcgimain, const gchar *cookie)
{
	ZakCgiMainPrivate *priv;

	GHashTable *ht;

	GValue *ret;

	g_return_val_if_fail (ZAK_CGI_IS_MAIN (zakcgimain), NULL);

	ht = _zak_cgi_main_get_cookies (zakcgimain);

	ret = (GValue *)g_hash_table_lookup (ht, cookie);

	return ret;
}

/**
 * zak_cgi_main_cookies_foreach:
 * @zakcgimain:
 * @func:
 * @user_data:
 *
 */
void
zak_cgi_main_cookies_foreach (ZakCgiMain *zakcgimain, GHFunc func, gpointer user_data)
{
	ZakCgiMainPrivate *priv;

	GHashTable *ht;

	g_return_if_fail (ZAK_CGI_IS_MAIN (zakcgimain));
	g_return_if_fail (func != NULL);

	ht = _zak_cgi_main_get_cookies (zakcgimain);

	g_hash_table_foreach (ht, func, user_data);
}

/**
 * zak_cgi_main_dump_cookies:
 * @zakcgimain:
 *
 * Returns: an html table with each cookies.
 */
G_DEPRECATED_FOR (zak_cgi_main_cookies_foreach)
gchar
*zak_cgi_main_dump_cookies (ZakCgiMain *zakcgimain)
{
	GHashTable *ht_env;
	GHashTableIter iter;

	GString *str;
	gchar *ret;

	gpointer key;
	gpointer value;

	ht_env = _zak_cgi_main_get_cookies (zakcgimain);

	str = g_string_new ("");

	if (g_hash_table_size (ht_env) > 0)
		{
			g_string_append_printf (str, "<table>\n");

			g_hash_table_iter_init (&iter, ht_env);
			while (g_hash_table_iter_next (&iter, &key, &value))
				{
					g_string_append_printf (str, "<tr><td>%s</td><td>%s</td></tr>\n",
					                        (gchar *)key, g_value_get_string ((GValue *)value));
				}

			g_string_append_printf (str, "</table>\n");
		}

	ret = g_strdup (str->str);
	g_string_free (str, TRUE);

	return ret;
}

/**
 * zak_cgi_main_set_cookie:
 * @name:
 * @value:
 * @expires:
 * @domain:
 * @path:
 * @secure:
 * @http_only:
 *
 * Returns: the string to set the cookie.
 */
gchar
*zak_cgi_main_set_cookie (const gchar *name,
                          const gchar *value,
                          GDateTime *expires,
                          const gchar *domain,
                          const gchar *path,
                          gboolean secure,
                          gboolean http_only)
{
	gchar *ret;
	GString *str;

	char *cur = g_strdup (setlocale (LC_TIME, NULL));

	setlocale (LC_NUMERIC, "C");

	str = g_string_new ("Set-Cookie: ");
	g_string_append_printf (str, "%s=%s",
							name,
							value);
	if (expires != NULL)
		{
			g_string_append_printf (str, "%s", g_date_time_format (expires, "; expires=%a, %d-%b-%Y %H:%M:%S GMT"));
		}
	if (domain != NULL)
		{
			g_string_append_printf (str, "; domain=%s", domain);
		}
	if (path != NULL)
		{
			g_string_append_printf (str, "; path=%s", path);
		}
	if (secure)
		{
			g_string_append (str, "; secure");
		}
	if (http_only)
		{
			g_string_append (str, "; httponly");
		}

	setlocale (LC_TIME, cur);
	g_free (cur);

	ret = g_strdup (str->str);
	g_string_free (str, TRUE);

	return ret;
}

/* from libsoup */
#define XDIGIT(c) ((c) <= '9' ? (c) - '0' : ((c) & 0x4F) - 'A' + 10)
#define HEXCHAR(s) ((XDIGIT (s[1]) << 4) + XDIGIT (s[2]))

static gboolean
form_decode (char *part)
{
	unsigned char *s, *d;

	s = d = (unsigned char *)part;
	do {
		if (*s == '%') {
			if (!g_ascii_isxdigit (s[1]) ||
			    !g_ascii_isxdigit (s[2]))
				return FALSE;
			*d++ = HEXCHAR (s);
			s += 2;
		} else if (*s == '+')
			*d++ = ' ';
		else
			*d++ = *s;
	} while (*s++);

	return TRUE;
}
/* end from libsoup */

static GHashTable
*_zak_cgi_main_get_parameters (ZakCgiMain *zakcgimain, const gchar *query_string)
{
	ZakCgiMainPrivate *priv;
	GHashTable *ht;

	const gchar *qstring;

	gchar **params;
	gchar **parts;
	guint i;
	guint l;
	gchar *name;
	gchar *value;

	GValue *gval;
	GPtrArray *ar;

	ht = NULL;

	if (zakcgimain != NULL)
		{
			priv = ZAK_CGI_MAIN_GET_PRIVATE (zakcgimain);
			if (priv->ht_parameters != NULL)
				{
					ht = g_hash_table_ref (priv->ht_parameters);
					return ht;
				}
		}

	if (ht == NULL)
		{
			ht = g_hash_table_new (g_str_hash, g_str_equal);
			if (zakcgimain != NULL)
				{
					priv->ht_parameters = g_hash_table_ref (ht);
				}
		}

	if (query_string == NULL)
		{
			qstring = g_getenv ("QUERY_STRING");
		}
	else
		{
			qstring = query_string;
		}
	params = g_strsplit (qstring, "&", -1);
	l = g_strv_length (params);
	for (i = 0; i < l; i++)
		{
			parts = g_strsplit (params[i], "=", 2);

			gval = (GValue *)g_hash_table_lookup (ht, parts[0]);
			if (gval != NULL)
				{
					if (!G_VALUE_HOLDS (gval, G_TYPE_PTR_ARRAY))
						{
							/* convert to GPtrArray */
							ar = g_ptr_array_new ();

							g_ptr_array_add (ar, g_strdup (g_value_get_string (gval)));
							g_value_unset (gval);

							g_value_init (gval, G_TYPE_PTR_ARRAY);
							g_value_take_boxed (gval, ar);
							name = g_strdup (parts[0]);
							form_decode (name);
							g_hash_table_replace (ht, g_strdup (name), gval);
							g_free (name);
						}
					else
						{
							ar = (GPtrArray *)g_value_get_boxed (gval);
						}
					if (parts[1] == NULL
					    || g_strcmp0 (parts[1], "") == 0)
						{
							g_ptr_array_add (ar, g_strdup (""));
						}
					else
						{
							value = g_strdup (parts[1]);
							form_decode (value);
							g_ptr_array_add (ar, g_strdup (value));
							g_free (value);
						}
				}
			else
				{
					gval = g_new0 (GValue, 1);

					g_value_init (gval, G_TYPE_STRING);

					if (parts[1] == NULL
					    || g_strcmp0 (parts[1], "") == 0)
						{
							g_value_set_string (gval, g_strdup (""));
						}
					else
						{
							value = g_strdup (parts[1]);
							form_decode (value);
							g_value_set_string (gval, g_strdup (value));
							g_free (value);
						}

					name = g_strdup (parts[0]);
					form_decode (name);
					g_hash_table_replace (ht, g_strdup (name), gval);
					g_free (name);
				}
			g_strfreev (parts);
		}
	g_strfreev (params);

	return ht;
}

/**
 * zak_cgi_main_get_parameters:
 * @zakcgimain:
 * @query_string:
 *
 * Returns:
 */
G_DEPRECATED_FOR (zak_cgi_main_get_parameter)
GHashTable
*zak_cgi_main_get_parameters (ZakCgiMain *zakcgimain, const gchar *query_string)
{
	return _zak_cgi_main_get_parameters (zakcgimain, query_string);
}

/**
 * zak_cgi_main_get_parameter:
 * @zakcgimain:
 * @param:
 *
 * Returns:
 */
GValue
*zak_cgi_main_get_parameter  (ZakCgiMain *zakcgimain, const gchar *param)
{
	ZakCgiMainPrivate *priv;

	GHashTable *ht;

	GValue *ret;

	g_return_val_if_fail (ZAK_CGI_IS_MAIN (zakcgimain), NULL);

	ht = _zak_cgi_main_get_parameters (zakcgimain, NULL);

	ret = g_hash_table_lookup (ht, param);

	return ret;
}

/**
 * zak_cgi_main_parameters_foreach:
 * @zakcgimain:
 * @func:
 * @user_data:
 *
 */
void
zak_cgi_main_parameters_foreach (ZakCgiMain *zakcgimain, GHFunc func, gpointer user_data)
{
	ZakCgiMainPrivate *priv;

	GHashTable *ht;

	g_return_if_fail (ZAK_CGI_IS_MAIN (zakcgimain));
	g_return_if_fail (func != NULL);

	ht = _zak_cgi_main_get_parameters (zakcgimain, NULL);

	g_hash_table_foreach (ht, func, user_data);
}

/**
 * zak_cgi_main_get_stdin:
 * @zakcgimain:
 *
 * Returns: the stdin.
 */
gchar
*zak_cgi_main_get_stdin (ZakCgiMain *zakcgimain)
{
	gchar *ret;

	ZakCgiMainPrivate *priv;

	const gchar *env;
	guint l;
	gsize bytesread;

	GError *error;
	GInputStream *istream;

	ret = NULL;
	if (zakcgimain != NULL)
		{
			priv = ZAK_CGI_MAIN_GET_PRIVATE (zakcgimain);
			if (priv->stdin != NULL)
				{
					return priv->stdin;
				}
		}

	env = g_getenv ("CONTENT_LENGTH");
	if (env != NULL)
		{
			l = strtol (env, NULL, 10);
			if (l > 0)
				{
					error = NULL;

					ret = g_malloc0 (l + 1);
					istream = g_unix_input_stream_new (0, TRUE);

					g_input_stream_read_all (istream,
					                         ret,
					                         l,
					                         &bytesread,
					                         NULL,
					                         &error);
					if (l != bytesread)
						{
							g_warning ("Error reading stdin: bytes read (%d) differ from content length (%d).", bytesread, l);
						}
					if (error != NULL)
						{
							g_warning ("Error reading stdin: %s", error->message);
						}
				}
		}

	if (zakcgimain != NULL)
		{
			priv->stdin = g_memdup (ret, bytesread);
		}

	return ret;
}

typedef struct
{
	gchar *data;
	gsize length;
} Block;

static GPtrArray
*zak_cgi_main_split_content (const gchar *buf,
                             guint l,
                             const gchar *delimiter)
{
	guint i;
	guint l_delimiter;
	guint start;

	gchar *_delimiter;

	GPtrArray *ar;

	gsize length;

	_delimiter = g_strdup_printf ("%s%c%c", delimiter, 13, 10);

	ar = g_ptr_array_new ();

	l_delimiter = strlen (_delimiter);

	start = 0;
	for (i = 0; i < (l - (l_delimiter + 2)); i++)
		{
			/* start to control if delimiter is reached */
			if (i >= l_delimiter
				&& memcmp (buf + (i - l_delimiter), _delimiter, l_delimiter) == 0)
				{
					Block *b;

					length = i - l_delimiter - start;

					if (length > 0)
						{
							b = g_new0 (Block, 1);

							b->data = g_memdup (buf + start, length);
							b->length = length;

							g_ptr_array_add (ar, b);
						}

					start = i;
				}
		}

	if (i >= l_delimiter)
		{
			Block *b;

			length = i - start;

			b = g_new0 (Block, 1);

			b->data = g_memdup (buf + start, length);
			b->length = length;

			g_ptr_array_add (ar, b);
		}

	return ar;
}

ZakCgiFile
*zak_cgi_file_copy (ZakCgiFile *file)
{
	ZakCgiFile *b;

	b = g_slice_new (ZakCgiFile);
	b->name = g_strdup (file->name);
	b->content = g_memdup (file->content, file->size);
	b->size = file->size;
	b->content_type = g_strdup (file->content_type);

	return b;
}

void
zak_cgi_file_free (ZakCgiFile *file)
{
	g_free (file->name);
	g_free (file->content);
	g_free (file->content_type);
	g_slice_free (ZakCgiFile, file);
}

G_DEFINE_BOXED_TYPE (ZakCgiFile, zak_cgi_file, zak_cgi_file_copy, zak_cgi_file_free)


static GHashTable
*_zak_cgi_main_parse_stdin (const gchar *buf, const gchar *boundary)
{
	GHashTable *ht;

	const gchar *env;

	guint l;

	gchar *_boundary;

	gchar *content_disposition;
	gchar *content_type;
	gchar *tmp;

	gchar **splitted;

	GPtrArray *content_splitted;
	guint j;

	gchar **boundary_splitted;

	gchar **v_content;
	gchar **parts;
	guint l_v_content;

	gchar *param_name;
	gchar *param_name_file;
	gchar *param_value;
	guint file_l;

	GValue *gval;
	GValue *gval_tmp;

	ZakCgiFile *zgfile;

	GPtrArray *ar;

	ht = NULL;

	env = g_getenv ("CONTENT_LENGTH");
	if (env != NULL)
		{
			l = strtol (env, NULL, 10);
			if (l < 1)
				{
					return ht;
				}
		}
	else
		{
			return ht;
		}

	content_type = g_strdup (g_getenv ("CONTENT_TYPE"));
	if (content_type == NULL)
		{
			return ht;
		}

	splitted = g_strsplit (content_type, ";", -1);
	if (g_strcmp0 (content_type, "") == 0
		|| g_strcmp0 (splitted[0], "application/x-www-form-urlencoded") == 0)
		{
			return _zak_cgi_main_get_parameters (NULL, buf);
		}
	else if (g_strcmp0 (splitted[0], "multipart/form-data") == 0)
		{
			if (g_strv_length (splitted) > 1
				&& boundary == NULL)
				{
					boundary_splitted = g_strsplit (splitted[1], "=", 2);
					_boundary = g_strdup_printf ("--%s", boundary_splitted[1]);
					g_strfreev (boundary_splitted);
				}
			else
				{
					if (boundary != NULL)
						{
							_boundary = g_strdup_printf ("--%s", boundary);
						}
				}
			g_strfreev (splitted);

			ht = g_hash_table_new (g_str_hash, g_str_equal);

			content_splitted = zak_cgi_main_split_content (buf, l, _boundary);

			for (j = 0; j < content_splitted->len; j++)
				{
					GMemoryInputStream *mstream;
					GDataInputStream *dataistream;
					gsize length;
					GError *error;

					Block *b;

					b = (Block *)g_ptr_array_index (content_splitted, j);

					if (b->length == 0
						|| memcmp (b->data, "--", 2) == 0)
						{
							continue;
						}

					/* read line */
					mstream = G_MEMORY_INPUT_STREAM (g_memory_input_stream_new_from_data (b->data, b->length, NULL));
					dataistream = g_data_input_stream_new (G_INPUT_STREAM (mstream));

					/* content-disposition */
					error = NULL;
					content_disposition = g_data_input_stream_read_line (dataistream, NULL, NULL, &error);
					if (content_disposition == NULL)
						{
							if (error != NULL)
								{
									g_warning ("Error on read content disposition: %s", error->message);
								}
							else
								{
									g_warning ("No content disposition found.");
								}
						}

					v_content = g_strsplit (content_disposition, ";", -1);
					l_v_content = g_strv_length (v_content);

					parts = g_strsplit_set (v_content[1], "=\"", 0);
					param_name = g_strdup (parts[2]);
					g_strfreev (parts);

					if (l_v_content == 3)
						{
							parts = g_strsplit_set (v_content[2], "=\"", 0);
							param_name_file = g_strdup (parts[2]);
							g_strfreev (parts);

							/* content-type */
							/* if (content_type != NULL) */
							/* 	{ */
							/* 		g_free (content_type); */
							/* 		content_type = NULL; */
							/* 	} */
							tmp = g_data_input_stream_read_line (dataistream, &length, NULL, NULL);
							tmp[length - 1] = '\0';
							parts = g_strsplit (tmp, ": ", -1);
							content_type = g_strdup (parts[1]);
							g_strfreev (parts);
							g_free (tmp);
						}
					else
						{
							/* if (content_type != NULL) */
							/* 	{ */
							/* 		g_free (content_type); */
									content_type = NULL;
							/* 	} */
						}

					g_free (content_disposition);
					g_strfreev (v_content);

					/* empty */
					g_data_input_stream_read_line (dataistream, NULL, NULL, NULL);

					if (l_v_content == 3)
						{
							param_value = (gchar *)g_malloc (1);
							if (g_strcmp0 (param_name_file, "") != 0)
								{
									tmp = g_data_input_stream_read_upto (dataistream, "", -1, &length, NULL, NULL);

									param_value = g_realloc (param_value, length);
									memmove (param_value, tmp, length);

									file_l = length;
								}
							else
								{
									/* empty */
									g_data_input_stream_read_line (dataistream, NULL, NULL, NULL);

									file_l = 2;
									param_value[0] = '\0';
								}
						}
					else
						{
							param_value = g_data_input_stream_read_line (dataistream, &length, NULL, NULL);
							param_value[length - 1] = '\0';
						}

					gval_tmp = g_new0 (GValue, 1);
					if (l_v_content == 3)
						{
							zgfile = (ZakCgiFile *)g_new0 (ZakCgiFile, 1);
							zgfile->name = g_strdup (param_name_file);
							zgfile->content = g_memdup (param_value, file_l - 2);
							zgfile->size = file_l - 2;
							zgfile->content_type = g_strdup (content_type);
							g_free (content_type);

							g_value_init (gval_tmp, ZAK_CGI_TYPE_FILE);
							g_value_take_boxed (gval_tmp, zgfile);
						}
					else
						{
							g_value_init (gval_tmp, G_TYPE_STRING);
							g_value_set_string (gval_tmp, g_strdup (param_value));
						}

					gval = g_hash_table_lookup (ht, param_name);
					if (gval != NULL)
						{
							if (!G_VALUE_HOLDS (gval, G_TYPE_PTR_ARRAY))
								{
									GValue *g;

									g = (GValue *)g_new0 (GValue, 1);
									g_value_init (g, G_VALUE_TYPE (gval));
									g_value_copy (gval, g);
									g_value_unset (gval);

									/* converto to GPtrArray */
									ar = g_ptr_array_new ();

									g_ptr_array_add (ar, g);

									g_value_init (gval, G_TYPE_PTR_ARRAY);
									g_value_take_boxed (gval, ar);
									g_hash_table_replace (ht, g_strdup (param_name), gval);
								}
							else
								{
									ar = (GPtrArray *)g_value_get_boxed (gval);
								}
							g_ptr_array_add (ar, gval_tmp);
						}
					else
						{
							g_hash_table_replace (ht, g_strdup (param_name), gval_tmp);
						}

					g_free (param_name);
					g_free (param_value);
					if (l_v_content == 3)
						{
							g_free (param_name_file);
						}
				}

			g_free (_boundary);
		}

	return ht;
}

/**
 * zak_cgi_main_parse_stdin:
 *
 * Returns:
 */
G_DEPRECATED_FOR (zak_cgi_main_get_stdin_field)
GHashTable
*zak_cgi_main_parse_stdin (const gchar *buf, const gchar *boundary)
{
	return _zak_cgi_main_parse_stdin (buf, boundary);
}

/**
 * zak_cgi_main_get_stdin_field:
 * @zakcgimain:
 * @field:
 *
 * Returns:
 */
GValue
*zak_cgi_main_get_stdin_field (ZakCgiMain *zakcgimain, const gchar *field)
{
	ZakCgiMainPrivate *priv;

	GHashTable *ht;

	GValue *ret;

	g_return_val_if_fail (ZAK_CGI_IS_MAIN (zakcgimain), NULL);

	ht = _zak_cgi_main_parse_stdin (zak_cgi_main_get_stdin (zakcgimain), NULL);

	ret = g_hash_table_lookup (ht, field);

	return ret;
}

/**
 * zak_cgi_main_stdin_foreach:
 * @zakcgimain:
 * @func:
 * @user_data:
 *
 */
void
zak_cgi_main_stdin_foreach (ZakCgiMain *zakcgimain, GHFunc func, gpointer user_data)
{
	ZakCgiMainPrivate *priv;

	GHashTable *ht;

	g_return_if_fail (ZAK_CGI_IS_MAIN (zakcgimain));
	g_return_if_fail (func != NULL);

	ht = _zak_cgi_main_parse_stdin (zak_cgi_main_get_stdin (zakcgimain), NULL);

	g_hash_table_foreach (ht, func, user_data);
}

/**
 * zak_cgi_main_redirect:
 * @zakcgimain:
 * @url:
 *
 */
void
zak_cgi_main_redirect (ZakCgiMain *zakcgimain, const gchar *url)
{
	GString *_url;
	GHashTable *ht_env;
	gchar *value;
	GRegex *regex;

	_url = g_string_new ("");
	if (!g_str_has_prefix (url, "http://")
		&& !g_str_has_prefix (url, "https://")
		&& !g_str_has_prefix (url, "ftp://"))
		{
			ht_env = _zak_cgi_main_get_env (zakcgimain);

			value = (gchar *)g_value_get_string ((GValue *)g_hash_table_lookup (ht_env, "REQUEST_SCHEME"));
			if (value != NULL)
				{
					g_string_append_printf (_url, "%s://", value);
				}

			value = (gchar *)g_value_get_string ((GValue *)g_hash_table_lookup (ht_env, "SERVER_NAME"));
			if (value != NULL)
				{
					if (!g_str_has_prefix (url, value))
						{
							g_string_append_printf (_url, value);
						}
				}

			if (!g_str_has_prefix (url, "/"))
				{
					/* TODO
					 * test if it starts with a domain, ex: www.google.it */
					value = (gchar *)g_value_get_string ((GValue *)g_hash_table_lookup (ht_env, "CONTEXT_PREFIX"));
					if (value != NULL)
						{
							if (!g_str_has_prefix (url, value))
								{
									g_string_append_printf (_url, value);
								}
						}
				}

			g_string_append_printf (_url, url);
		}
	else
		{
			g_string_assign (_url, url);
		}

	g_printf ("%s%s%c%c\n",
	          "Location: ",
	          _url->str,
	          13, 10);

	g_string_free (_url, TRUE);
}

/**
 * zak_cgi_main_is_request_method:
 * @zakcgimain:
 * @method:
 *
 * Returns:
 */
gboolean
zak_cgi_main_is_request_method (ZakCgiMain *zakcgimain, const gchar *method)
{
	gboolean ret;

	GValue *gval;
	const gchar *param;

	ret = FALSE;

	gval = zak_cgi_main_get_env_field (zakcgimain, "REQUEST_METHOD");
	if (gval != NULL)
		{
			param = g_value_get_string (gval);
			ret = (g_strcmp0 (param, method) == 0);
		}

	return ret;
}

/**
 * zak_cgi_main_is_get:
 * @zakcgimain:
 *
 * Returns:
 */
gboolean
zak_cgi_main_is_get (ZakCgiMain *zakcgimain)
{
	return zak_cgi_main_is_request_method (zakcgimain, "GET");
}

/**
 * zak_cgi_main_is_post:
 * @zakcgimain:
 *
 * Returns:
 */
gboolean
zak_cgi_main_is_post (ZakCgiMain *zakcgimain)
{
	return zak_cgi_main_is_request_method (zakcgimain, "POST");
}

/* PRIVATE */
static void
zak_cgi_main_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiMain *zak_cgi_main = (ZakCgiMain *)object;
	ZakCgiMainPrivate *priv = ZAK_CGI_MAIN_GET_PRIVATE (zak_cgi_main);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_main_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiMain *zak_cgi_main = (ZakCgiMain *)object;
	ZakCgiMainPrivate *priv = ZAK_CGI_MAIN_GET_PRIVATE (zak_cgi_main);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_main_dispose (GObject *gobject)
{
	ZakCgiMain *zak_cgi_main = (ZakCgiMain *)gobject;
	ZakCgiMainPrivate *priv = ZAK_CGI_MAIN_GET_PRIVATE (zak_cgi_main);


	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->dispose (gobject);
}

static void
zak_cgi_main_finalize (GObject *gobject)
{
	ZakCgiMain *zak_cgi_main = (ZakCgiMain *)gobject;
	ZakCgiMainPrivate *priv = ZAK_CGI_MAIN_GET_PRIVATE (zak_cgi_main);

	if (priv->ht_env != NULL)
		{
			g_hash_table_destroy (priv->ht_env);
		}
	if (priv->ht_cookies != NULL)
		{
			g_hash_table_destroy (priv->ht_cookies);
		}
	if (priv->ht_parameters != NULL)
		{
			g_hash_table_destroy (priv->ht_parameters);
		}
	if (priv->stdin != NULL)
		{
			g_free (priv->stdin);
		}

	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->finalize (gobject);
}
