/*
 * Copyright (C) 2015-2016 Andrea Zagli <azagli@libero.it>
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

#include <sys/stat.h>
#include <syslog.h>

#include <glib/gstdio.h>
#include <gio/gio.h>

#include <string.h>

#include "session.h"

enum
{
	PROP_0,
	PROP_MINUTES,
};

static void zak_cgi_session_class_init (ZakCgiSessionClass *class);
static void zak_cgi_session_init (ZakCgiSession *zak_cgi_session);

static void zak_cgi_session_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_cgi_session_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

static void zak_cgi_session_dispose (GObject *gobject);
static void zak_cgi_session_finalize (GObject *gobject);

static gchar *zak_cgi_session_build_filename (ZakCgiSession *session);
static void zak_cgi_session_create_file (ZakCgiSession *session);

#define ZAK_CGI_SESSION_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_CGI_TYPE_SESSION, ZakCgiSessionPrivate))

#define MINUTES_DEFAULT 5

typedef struct _ZakCgiSessionPrivate ZakCgiSessionPrivate;
struct _ZakCgiSessionPrivate
	{
		ZakCgiMain *zakcgimain;
		gchar *base_uri;
		gchar *path;
		gchar *sid;
		GFile *gfile;
		GKeyFile *kfile;

		gint minutes;
	};

G_DEFINE_TYPE (ZakCgiSession, zak_cgi_session, G_TYPE_OBJECT)

static void
zak_cgi_session_class_init (ZakCgiSessionClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = zak_cgi_session_set_property;
	object_class->get_property = zak_cgi_session_get_property;
	object_class->dispose = zak_cgi_session_dispose;
	object_class->finalize = zak_cgi_session_finalize;

	g_object_class_install_property (object_class, PROP_MINUTES,
	                                 g_param_spec_int ("minutes",
													   "Minutes",
													   "Minutes",
													   -1,
													   G_MAXINT,
													   MINUTES_DEFAULT,
													   G_PARAM_READABLE));

	g_type_class_add_private (object_class, sizeof (ZakCgiSessionPrivate));
}

static void
zak_cgi_session_init (ZakCgiSession *zak_cgi_session)
{
	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (zak_cgi_session);

	priv->zakcgimain = NULL;
	priv->base_uri = NULL;
	priv->path = NULL;
	priv->gfile = NULL;
	priv->kfile = NULL;

	priv->minutes = MINUTES_DEFAULT;
}

/**
 * zak_cgi_session_new:
 * @zakcgimain:
 * @base_uri:
 * @path:
 * @minutes:
 *
 * Returns: the newly created #ZakCgiSession object.
 */
ZakCgiSession
*zak_cgi_session_new_minutes (ZakCgiMain *zakcgimain,
							  const gchar *base_uri,
							  const gchar *path,
							  gint minutes)
{
	GHashTable *ht_cookies;

	GDateTime *gdt_now;
	GDateTime *gdt_exp;

	ZakCgiSession *zak_cgi_session;
	ZakCgiSessionPrivate *priv;

	zak_cgi_session = ZAK_CGI_SESSION (g_object_new (zak_cgi_session_get_type (), NULL));

	priv = ZAK_CGI_SESSION_GET_PRIVATE (zak_cgi_session);
	priv->zakcgimain = zakcgimain;
	if (base_uri != NULL)
		{
			priv->base_uri = g_strdup (base_uri);
		}
	if (path != NULL)
		{
			priv->path = g_strdup (path);
		}

	priv->minutes = MAX (-1, minutes);

	ht_cookies = zak_cgi_main_get_cookies (priv->zakcgimain);
	priv->sid = g_strdup (g_value_get_string (g_hash_table_lookup (ht_cookies, "ZAKCGISID")));

	if (zak_cgi_session_is_valid (zak_cgi_session))
		{
			/* update timestamp */
			gdt_now = g_date_time_new_now_local ();
			gdt_exp = g_date_time_add_minutes (gdt_now, priv->minutes);
			g_key_file_set_string (priv->kfile, "ZAKCGI", "TIMESTAMP", g_date_time_format (gdt_now, "%FT%T"));
			g_key_file_set_string (priv->kfile, "ZAKCGI", "TIMESTAMP_EXPIRES", g_date_time_format (gdt_exp, "%FT%T"));
			g_key_file_save_to_file (priv->kfile, g_file_get_path (priv->gfile), NULL);
			g_date_time_unref (gdt_exp);
			g_date_time_unref (gdt_now);

			g_chmod (g_file_get_path (priv->gfile), S_IRUSR | S_IWUSR);
		}
	else
		{
			if (priv->sid != NULL)
				{
					zak_cgi_session_close (zak_cgi_session);
				}

			zak_cgi_session_create_file (zak_cgi_session);
		}

	return zak_cgi_session;
}

/**
 * zak_cgi_session_new:
 * @zakcgimain:
 * @base_uri:
 * @path:
 *
 * Returns: the newly created #ZakCgiSession object.
 */
ZakCgiSession
*zak_cgi_session_new (ZakCgiMain *zakcgimain,
					  const gchar *base_uri,
					  const gchar *path)
{
	return zak_cgi_session_new_minutes (zakcgimain, base_uri, path, MINUTES_DEFAULT);
}

/**
 * zak_cgi_session_get_minutes:
 * @session:
 *
 */
gint
zak_cgi_session_get_minutes (ZakCgiSession *session)
{
	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	return priv->minutes;
}

/**
 * zak_cgi_session_get_header:
 * @session:
 *
 * Returns: the header that set the cookie session, if needed; else an empty string.
 */
gchar
*zak_cgi_session_get_header (ZakCgiSession *session)
{
	gchar *ret;

	GHashTable *ht_env;

	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	if (priv->sid == NULL)
		{
			zak_cgi_session_create_file (session);
		}

	if (priv->sid != NULL)
		{
			ht_env = zak_cgi_main_get_env (priv->zakcgimain);

			ret = zak_cgi_main_set_cookie ("ZAKCGISID", priv->sid, NULL, NULL,
										   priv->base_uri != NULL ? priv->base_uri : (gchar *)g_hash_table_lookup (ht_env, "CONTEXT_PREFIX"),
										   FALSE, FALSE);
		}
	else
		{
			ret = g_strdup ("");
		}

	return ret;
}

/**
 * zak_cgi_session_set_value:
 * @session:
 * @name:
 * @value:
 *
 */
void
zak_cgi_session_set_value (ZakCgiSession *session, const gchar *name, const gchar *value)
{
	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	if (priv->kfile != NULL)
		{
			g_key_file_set_string (priv->kfile, "SESSION", name, value);
			g_key_file_save_to_file (priv->kfile, g_file_get_path (priv->gfile), NULL);
			g_chmod (g_file_get_path (priv->gfile), S_IRUSR | S_IWUSR);
		}
}

/**
 * zak_cgi_session_get_value:
 * @session:
 * @name:
 *
 * Returns: a value from session.
 */
gchar
*zak_cgi_session_get_value (ZakCgiSession *session, const gchar *name)
{
	gchar *ret;

	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	ret = NULL;
	if (priv->kfile != NULL)
		{
			ret = g_key_file_get_string (priv->kfile, "SESSION", name, NULL);
		}

	return ret;
}

/**
 * zak_cgi_session_is_valid:
 * @session:
 *
 */
gboolean
zak_cgi_session_is_valid (ZakCgiSession *session)
{
	gboolean ret;

	GError *error;

	gchar *filename;
	gchar *val;

	GTimeVal tval;
	GDateTime *gdt;
	GDateTime *gdt_now;
	GDateTime *gdt_plus;

	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	ret = FALSE;

	if (priv->sid != NULL)
		{
			/* open the file */
			filename = zak_cgi_session_build_filename (session);
			priv->gfile = g_file_new_for_path (filename);
			g_free (filename);

			error = NULL;

			/* check the content */
			priv->kfile = g_key_file_new ();
			if (g_key_file_load_from_file (priv->kfile,
			                                g_file_get_path (priv->gfile),
			                                G_KEY_FILE_NONE,
			                                &error)
			    && error == NULL)
				{
					val = g_key_file_get_string (priv->kfile, "ZAKCGI", "REMOTE_ADDR", NULL);
					if (val != NULL
						&& g_strcmp0 (val, g_getenv ("REMOTE_ADDR")) == 0)
						{
							/* if priv->minutes <= -1 the session never expires */
							if (priv->minutes > -1)
								{
									val = g_key_file_get_string (priv->kfile, "ZAKCGI", "TIMESTAMP", NULL);
									if (val != NULL)
										{
											if (g_time_val_from_iso8601 (val, &tval))
												{
													gdt = g_date_time_new_from_timeval_local (&tval);
													if (gdt != NULL)
														{
															gdt_plus = g_date_time_add_minutes (gdt, priv->minutes);
															gdt_now = g_date_time_new_now_local ();
															if (g_date_time_compare (gdt_plus, gdt_now) > -1)
																{
																	ret = TRUE;
																}
															else
																{
																	g_warning ("Session expired.");
																}

															g_date_time_unref (gdt_plus);
															g_date_time_unref (gdt_now);
														}
													else
														{
															g_warning ("Session expired.");
														}

													g_date_time_unref (gdt);
												}
											else
												{
													g_warning ("Session expired.");
												}
										}
									else
										{
											g_warning ("Session expired.");
										}
								}
						}
					else
						{
							g_warning ("Possibile session hijackin: right addr %s; current addr %s",
									   val, g_getenv ("REMOTE_ADDR"));
						}
				}
			else
				{
					g_warning ("Unable to open session file «%s»: %s.",
							   g_file_get_path (priv->gfile),
							   error != NULL && error->message != NULL ? error->message : "no details");
				}
		}

	return ret;
}

/**
 * zak_cgi_session_close:
 * @session:
 *
 */
void
zak_cgi_session_close (ZakCgiSession *session)
{
	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	if (priv->kfile != NULL)
		{
			g_key_file_free (priv->kfile);
			priv->kfile = NULL;
		}
	if (priv->gfile != NULL)
		{
			g_file_delete (priv->gfile, NULL, NULL);
			g_object_unref (priv->gfile);
			priv->gfile = NULL;
		}
	if (priv->sid != NULL)
		{
			g_free (priv->sid);
			priv->sid = NULL;
		}
}

/* PRIVATE */
static void
zak_cgi_session_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiSession *zak_cgi_session = (ZakCgiSession *)object;
	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (zak_cgi_session);

	switch (property_id)
		{
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
		}
}

static void
zak_cgi_session_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiSession *zak_cgi_session = (ZakCgiSession *)object;
	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (zak_cgi_session);

	switch (property_id)
		{
		case PROP_MINUTES:
			g_value_set_int (value, zak_cgi_session_get_minutes (zak_cgi_session));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
		}
}

static void
zak_cgi_session_dispose (GObject *gobject)
{
	ZakCgiSession *zak_cgi_session = (ZakCgiSession *)gobject;
	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (zak_cgi_session);


	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->dispose (gobject);
}

static void
zak_cgi_session_finalize (GObject *gobject)
{
	ZakCgiSession *zak_cgi_session = (ZakCgiSession *)gobject;
	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (zak_cgi_session);


	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->finalize (gobject);
}

static gchar
*zak_cgi_session_build_filename (ZakCgiSession *session)
{
	gchar *filename;
	gchar *tmp;

	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	tmp = g_strdup_printf ("%s.session", priv->sid);
	filename = g_build_filename (priv->path != NULL ? priv->path : g_get_tmp_dir (), "zakcgi", tmp, NULL);
	g_free (tmp);

	return filename;
}

static void
zak_cgi_session_create_file (ZakCgiSession *session)
{
	gchar *filename;

	GError *error;
	GFile *gf_path;
	GFileOutputStream *ostream;

	guint32 i;
	gchar *tmp;

	GDateTime *gdt;
	GDateTime *gdt_exp;

	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	/* create new random name */
	i = g_random_int ();

	tmp = g_strdup_printf ("%d", i);

	priv->sid = g_compute_checksum_for_string (G_CHECKSUM_MD5,
											   tmp,
											   strlen (tmp));

	g_free (tmp);

	filename = zak_cgi_session_build_filename (session);
	priv->gfile = g_file_new_for_path (filename);
	g_free (filename);

	/* check if path exists */
	gf_path = g_file_get_parent (priv->gfile);
	if (gf_path != NULL)
		{
			if (!g_file_query_exists (gf_path, NULL))
				{
					tmp = g_file_get_path (gf_path);
					if (g_mkdir_with_parents (tmp, S_IRWXU) < 0)
						{
							g_warning ("Unable to create the session file directory «%s».",
									   tmp);

							g_free (tmp);
							g_object_unref (gf_path);

							zak_cgi_session_close (session);

							return;
						}
					g_free (tmp);
				}
			g_object_unref (gf_path);
		}

	error = NULL;
	ostream = g_file_replace (priv->gfile, NULL, FALSE, G_FILE_CREATE_PRIVATE, NULL, &error);
	if (ostream == NULL
		|| error != NULL)
		{
			g_warning ("Unable to create the session file «%s»: %s.",
					   filename,
					   error != NULL && error->message != NULL ? error->message : "no details");
		}
	else
		{
			g_output_stream_close (G_OUTPUT_STREAM (ostream), NULL, NULL);
			g_object_unref (ostream);

			priv->kfile = g_key_file_new ();
			if (!g_key_file_load_from_file (priv->kfile,
											g_file_get_path (priv->gfile),
											G_KEY_FILE_NONE,
											&error)
				|| error != NULL)
				{
					/* TODO */
				}
			else
				{
					/* write REMOTE_ADDR and creation timestamp */
					gdt = g_date_time_new_now_local ();
					gdt_exp = g_date_time_add_minutes (gdt, priv->minutes);

					g_key_file_set_string (priv->kfile, "ZAKCGI", "REMOTE_ADDR", g_getenv ("REMOTE_ADDR"));
					g_key_file_set_string (priv->kfile, "ZAKCGI", "TIMESTAMP", g_date_time_format (gdt, "%FT%T"));
					g_key_file_set_string (priv->kfile, "ZAKCGI", "TIMESTAMP_EXPIRES", g_date_time_format (gdt_exp, "%FT%T"));
					g_key_file_save_to_file (priv->kfile, g_file_get_path (priv->gfile), NULL);

					g_date_time_unref (gdt_exp);
					g_date_time_unref (gdt);
				}

			g_chmod (g_file_get_path (priv->gfile), S_IRUSR | S_IWUSR);
		}
}
