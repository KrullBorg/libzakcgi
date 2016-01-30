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

#include <gio/gio.h>

#include <string.h>

#include "session.h"

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

#define ZAK_CGI_SESSION_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_CGI_TYPE_SESSION, ZakCgiSessionPrivate))

typedef struct _ZakCgiSessionPrivate ZakCgiSessionPrivate;
struct _ZakCgiSessionPrivate
	{
		ZakCgiMain *zakcgimain;
		gchar *base_uri;
		gchar *path;
		gchar *sid;
		GFile *gfile;
		GKeyFile *kfile;
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
	GError *error;
	gchar *val;

	ZakCgiSession *zak_cgi_session;
	ZakCgiSessionPrivate *priv;

	g_return_val_if_fail (ZAK_CGI_IS_MAIN (zakcgimain), NULL);

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

	priv->sid = g_strdup ((gchar *)g_value_get_string (zak_cgi_main_get_cookie (priv->zakcgimain, "ZAKCGISID")));

	if (priv->sid != NULL)
		{
			/* open the file */
			priv->gfile = g_file_new_for_path (g_build_filename (priv->path != NULL ? priv->path : g_get_tmp_dir (), priv->sid, NULL));

			error = NULL;

			/* check the content */
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
					val = g_key_file_get_value (priv->kfile, "ZAKCGI", "REMOTE_ADDR", NULL);
					if (val == NULL
						|| g_strcmp0 (val, g_getenv ("REMOTE_ADDR")) != 0)
						{
							zak_cgi_session_close (zak_cgi_session);
						}

					val = g_key_file_get_value (priv->kfile, "ZAKCGI", "TIMESTAMP", NULL);
					if (val == NULL)
						{
							zak_cgi_session_close (zak_cgi_session);
						}
					else
						{
							GTimeVal tval;

							if (g_time_val_from_iso8601 (val, &tval))
								{
									GDateTime *gdt;
									GDateTime *gdt_now;
									GDateTime *gdt_plus;

									gdt = g_date_time_new_from_timeval_local (&tval);
									if (gdt == NULL)
										{
											zak_cgi_session_close (zak_cgi_session);
										}
									else
										{
											/* TODO
											 * add a property for minutes number */
											gdt_plus = g_date_time_add_minutes (gdt, 5);
											gdt_now = g_date_time_new_now_local ();
											if (g_date_time_compare (gdt_plus, gdt_now) == -1)
												{
													/* session expired */
													zak_cgi_session_close (zak_cgi_session);
												}
											else
												{
													/* update timestamp */
													g_key_file_set_value (priv->kfile, "ZAKCGI", "TIMESTAMP", g_date_time_format (gdt_now, "%FT%T"));
													g_key_file_save_to_file (priv->kfile, g_file_get_path (priv->gfile), NULL);
												}

											g_date_time_unref (gdt_plus);
											g_date_time_unref (gdt_now);
										}

									g_date_time_unref (gdt);
								}
							else
								{
									zak_cgi_session_close (zak_cgi_session);
								}
						}
				}
		}

	return zak_cgi_session;
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

	GError *error;
	GFileIOStream *iostream;

	guint32 i;
	gchar *tmp;

	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	if (priv->sid == NULL)
		{
			/* create new random name */

			i = g_random_int ();

			tmp = g_strdup_printf ("%d", i);

			priv->sid = g_compute_checksum_for_string (G_CHECKSUM_MD5,
			                                           tmp,
			                                           strlen (tmp));

			g_free (tmp);

			/* see if file already exists */
			priv->gfile = g_file_new_for_path (g_build_filename (priv->path != NULL ? priv->path : g_get_tmp_dir (), priv->sid, NULL));
			error = NULL;
			iostream = g_file_replace_readwrite (priv->gfile, NULL, FALSE, G_FILE_CREATE_PRIVATE, NULL, &error);
			if (iostream == NULL
			    || error != NULL)
				{
					/* TODO */
					g_warning ("Unable to write new session file: %s.",
							   error != NULL && error->message != NULL ? error->message : "no details");
				}
			else
				{
					g_io_stream_close (G_IO_STREAM (iostream), NULL, NULL);
					g_object_unref (iostream);

					priv->kfile = g_key_file_new ();
					if (!g_key_file_load_from_file (priv->kfile,
													g_file_get_path (priv->gfile),
													G_KEY_FILE_NONE,
													&error)
						|| error != NULL)
						{
							/* TODO */
							g_warning ("Unable to read session file: %s.",
									   error != NULL && error->message != NULL ? error->message : "no details");
						}
					else
						{
							/* write REMOTE_ADDR and creation timestamp */
							GDateTime *gdt;

							gdt = g_date_time_new_now_local ();

							g_key_file_set_value (priv->kfile, "ZAKCGI", "REMOTE_ADDR", g_getenv ("REMOTE_ADDR"));
							g_key_file_set_value (priv->kfile, "ZAKCGI", "TIMESTAMP", g_date_time_format (gdt, "%FT%T"));
							g_key_file_save_to_file (priv->kfile, g_file_get_path (priv->gfile), NULL);

							g_date_time_unref (gdt);
						}
				}

			ret = zak_cgi_main_set_cookie ("ZAKCGISID", priv->sid, NULL, NULL,
										   priv->base_uri != NULL ? priv->base_uri : (gchar *)g_value_get_string (zak_cgi_main_get_env_field (priv->zakcgimain, "CONTEXT_PREFIX")),
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

	GError *error;

	if (priv->kfile != NULL)
		{
			g_key_file_set_value (priv->kfile, "SESSION", name, value);

			error = NULL;
			if (!g_key_file_save_to_file (priv->kfile, g_file_get_path (priv->gfile), &error)
				|| error != NULL)
				{
					g_warning ("Unable to write value tosession file: %s.",
							   error != NULL && error->message != NULL ? error->message : "no details");
				}
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
			ret = g_key_file_get_value (priv->kfile, "SESSION", name, NULL);
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
