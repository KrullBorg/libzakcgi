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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <stdio.h>
#include <string.h>

#include <syslog.h>

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
 * body:
 *
 */
void
zak_cgi_main_out (const gchar *body)
{
	g_printf ("%s%c%c\n%s\n",
	          "Content-Type: text/html; charset=UTF-8", 13, 10,
	          body);
}

/**
 * zak_cgi_main_get_env:
 *
 * Returns: a #GHashTable with all the environment variables.
 */
GHashTable
*zak_cgi_main_get_env (void)
{
	GHashTable *ht;

	gchar **environ;
	guint l;
	guint i;
	gchar **envs;

	ht = g_hash_table_new (g_str_hash, g_str_equal);

	environ = g_get_environ ();
	l = g_strv_length (environ);
	for (i = 0; i < l; i++)
		{
			envs = g_strsplit (environ[i], "=", 2);
			g_hash_table_replace (ht, g_strdup (envs[0]), g_strdup (envs[1]));
			g_strfreev (envs);
		}

	return ht;
}

/**
 * zak_cgi_main_dump_env:
 *
 * Returns: an html table with each environment variables.
 */
gchar
*zak_cgi_main_dump_env ()
{
	GHashTable *ht_env;
	GHashTableIter iter;

	GString *str;
	gchar *ret;

	gpointer key;
	gpointer value;

	ht_env = zak_cgi_main_get_env ();

	str = g_string_new ("");

	if (g_hash_table_size (ht_env) > 0)
		{
			g_string_append_printf (str, "<table>\n");

			g_hash_table_iter_init (&iter, ht_env);
			while (g_hash_table_iter_next (&iter, &key, &value))
				{
					g_string_append_printf (str, "<tr><td>%s</td><td>%s</td></tr>\n",
					                        (gchar *)key, (gchar *)value);
				}

			g_string_append_printf (str, "</table>\n");
		}

	ret = g_strdup (str->str);
	g_string_free (str, TRUE);

	return ret;
}

/**
 * zak_cgi_main_get_parameters:
 *
 * Returns:
 */
GHashTable
*zak_cgi_main_get_parameters (void)
{
	GHashTable *ht;

	const gchar *qstring;

	gchar **params;
	gchar **parts;
	guint i;
	guint l;

	ht = g_hash_table_new (g_str_hash, g_str_equal);

	qstring = g_getenv ("QUERY_STRING");
	params = g_strsplit (qstring, "&", -1);
	l = g_strv_length (params);
	for (i = 0; i < l; i++)
		{
			parts = g_strsplit (params[i], "=", 2);
			g_hash_table_replace (ht, g_strdup (parts[0]), g_strdup (parts[1]));
			g_strfreev (parts);
		}

	return ht;
}

/**
 * zak_cgi_main_get_stdin:
 *
 * Returns: the stdin.
 */
gchar
*zak_cgi_main_get_stdin (void)
{
	gchar *ret;

	const gchar *env;
	guint l;
	gsize bytesread;

	GError *error;
	GInputStream *istream;

	ret = NULL;

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
							syslog (LOG_MAKEPRI(LOG_SYSLOG, LOG_DEBUG), "error reading stdin: bytes read differ from content length");
						}
					if (error != NULL)
						{
							syslog (LOG_MAKEPRI(LOG_SYSLOG, LOG_DEBUG), "error reading stdin: %s", error->message);
						}

					syslog (LOG_MAKEPRI(LOG_SYSLOG, LOG_DEBUG), "stdin: %s", ret);
				}
		}

	return ret;
}

static gchar
*zak_cgi_main_read_line (const gchar *buf, guint l, guint *i,
                         gboolean no_new_line_but_null_terminated,
                         guint *bytesread)
{
	gchar *line;
	guint line_start;
	guint count;

	for (line_start = *i, count = 1; *i < l - 1; (*i)++, count++)
		{
			if (buf[*i] == 13 && buf[*i + 1] == 10)
				{
					(*i)++;
					count++;
					break;
				}
		}

	line = (gchar *)g_memdup (buf + line_start, count);
	if (no_new_line_but_null_terminated)
		{
			line[count - 2] = '\0';
		}

	*bytesread = count;

	(*i)++;

	return line;
}

/**
 * zak_cgi_main_parse_stdin:
 *
 * Returns:
 */
GHashTable
*zak_cgi_main_parse_stdin (const gchar *buf, const gchar *boundary)
{
	GHashTable *ht;

	const gchar *env;

	guint l;
	guint i;
	guint bytesread;

	gchar *_boundary;

	gchar *line;
	gchar *content_disposition;
	gchar *content_type;
	gchar *tmp;

	ht = NULL;

	env = g_getenv ("CONTENT_LENGTH");
	if (env != NULL)
		{
			l = strtol (env, NULL, 10);
			if (l > 0)
				{
					ht = g_hash_table_new (g_str_hash, g_str_equal);

					_boundary = g_strdup_printf ("--%s", boundary);

					i = 0;
					do
						{
							/* read line */
							line = zak_cgi_main_read_line (buf, l, &i, TRUE, &bytesread);
							syslog (LOG_MAKEPRI(LOG_SYSLOG, LOG_DEBUG), "boundary: %s %d", line, bytesread);
							if (g_strcmp0 (line, _boundary) == 0)
								{
									/* content-disposition */
									gchar **v_content;
									gchar **parts;
									guint l_v_content;

									gchar *param_name;
									gchar *param_name_file;
									gchar *param_value;
									guint file_l;

									GValue *gval;

									content_disposition = zak_cgi_main_read_line (buf, l, &i, TRUE, &bytesread);
									syslog (LOG_MAKEPRI(LOG_SYSLOG, LOG_DEBUG), "content_disposition: %s", content_disposition);

									v_content = g_strsplit (content_disposition, ";", -1);
									l_v_content = g_strv_length (v_content);

									parts = g_strsplit (v_content[1], "=", 2);
									param_name = g_strndup (parts[1] + 1, strlen (parts[1]) - 2);
									param_name[strlen (parts[1]) - 2] = '\0';
									g_strfreev (parts);

									if (l_v_content == 3)
										{
											parts = g_strsplit (v_content[2], "=", 2);
											param_name_file = g_strndup (parts[1] + 1, strlen (parts[1]) - 2);
											param_name_file[strlen (parts[1]) - 2] = '\0';
											syslog (LOG_MAKEPRI(LOG_SYSLOG, LOG_DEBUG), "param_name_file: %s", param_name_file);
											g_strfreev (parts);

											/* content-type */
											content_type = zak_cgi_main_read_line (buf, l, &i, TRUE, &bytesread);
											syslog (LOG_MAKEPRI(LOG_SYSLOG, LOG_DEBUG), "content_type: %s", content_type);
											g_free (content_type);
										}

									g_strfreev (v_content);
									g_free (content_disposition);

									/* empty */
									g_free (zak_cgi_main_read_line (buf, l, &i, TRUE, &bytesread));

									if (l_v_content == 3)
										{
											param_value = (gchar *)g_malloc (1);
											if (g_strcmp0 (param_name_file, "") != 0)
												{
													gboolean exit;

													exit = FALSE;
													file_l = 0;
													do
														{
															tmp = zak_cgi_main_read_line (buf, l, &i, FALSE, &bytesread);
															syslog (LOG_MAKEPRI(LOG_SYSLOG, LOG_DEBUG), "tmp: %s %d", tmp, bytesread);

															if (memcmp (tmp, _boundary, strlen (_boundary)) == 0)
																{
																	i -= bytesread;
																	exit = TRUE;
																}
															else
																{
																	param_value = g_realloc (param_value, file_l + bytesread);
																	memmove (param_value + file_l, tmp, bytesread);
																	file_l += bytesread;
																}

															g_free (tmp);
														} while (!exit);
												}
											else
												{
													/* empty */
													g_free (zak_cgi_main_read_line (buf, l, &i, TRUE, &bytesread));

													file_l = 3;
													param_value[0] = '\0';
												}
										}
									else
										{
											param_value = zak_cgi_main_read_line (buf, l, &i, TRUE, &bytesread);
											syslog (LOG_MAKEPRI(LOG_SYSLOG, LOG_DEBUG), "param_value: %s", param_value);
										}

									gval = g_new0 (GValue, 1);

									if (l_v_content == 3)
										{
											GPtrArray *ar;

											ar = g_ptr_array_new ();
											g_ptr_array_add (ar, g_strdup (param_name_file));
											g_ptr_array_add (ar, g_memdup (param_value, file_l - 2));
											g_ptr_array_add (ar, GSIZE_TO_POINTER (file_l - 2));

											g_value_init (gval, G_TYPE_PTR_ARRAY);
											g_value_take_boxed (gval, ar);
										}
									else
										{
											g_value_init (gval, G_TYPE_STRING);
											g_value_set_string (gval, g_strdup (param_value));
										}

									g_hash_table_replace (ht, g_strdup (param_name), gval);

									g_free (param_name);
									g_free (param_value);
									if (l_v_content == 3)
										{
											g_free (param_name_file);
										}
								}
							else
								{
									tmp = g_strdup_printf ("%s--", _boundary);
									if (g_strcmp0 (line, tmp) == 0)
										{
											g_free (tmp);
											break;
										}
									g_free (tmp);
								}

							g_free (line);
						} while (i < l);
					
					g_free (_boundary);
				}
		}
	
	return ht;
}

void
zak_cgi_main_redirect (const gchar *url)
{
	g_printf ("%s%s%c%c\n",
	          "Location: ",
	          url,
	          13, 10);
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


	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->finalize (gobject);
}
