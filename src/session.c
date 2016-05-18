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

#include <stdlib.h>

#include <gio/gio.h>

#include <string.h>

#include <libzakformini/libzakformini.h>

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
		ZakFormIniProvider *zakformini;

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
	priv->zakformini = NULL;

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
	GDateTime *gdt_now;

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
	priv->minutes = MAX (-1, minutes);

	if (zak_cgi_session_is_valid (zak_cgi_session))
		{
			/* update timestamp */
			gdt_now = g_date_time_new_now_local ();
			g_key_file_set_string (priv->kfile, "ZAKCGI", "TIMESTAMP", g_date_time_format (gdt_now, "%FT%T"));
			g_key_file_save_to_file (priv->kfile, g_file_get_path (priv->gfile), NULL);
			g_date_time_unref (gdt_now);
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
 * zak_cgi_session_set_value_full:
 * @session:
 * @group:
 * @name:
 * @value:
 *
 */
void
zak_cgi_session_set_value_full (ZakCgiSession *session, const gchar *group, const gchar *name, const gchar *value)
{
	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	GError *error;

	if (priv->kfile != NULL)
		{
			if (name == NULL)
				{
					error = NULL;
					if (!g_key_file_remove_group (priv->kfile, group, &error)
						|| error != NULL)
						{
							g_warning ("Unable to unset key «%s» in group «%s»: %s.",
									   name,
									   group,
									   error != NULL && error->message != NULL ? error->message : "no details");
						}
				}
			else
				{
					if (value == NULL)
						{
							error = NULL;
							if (!g_key_file_remove_key (priv->kfile, group, name, &error)
								|| error != NULL)
								{
									g_warning ("Unable to unset key «%s» in group «%s»: %s.",
											   name,
											   group,
											   error != NULL && error->message != NULL ? error->message : "no details");
								}
						}
					else
						{
							g_key_file_set_string (priv->kfile, group, name, value);
						}
				}

			error = NULL;
			if (!g_key_file_save_to_file (priv->kfile, g_file_get_path (priv->gfile), &error)
				|| error != NULL)
				{
					g_warning ("Unable to write session file: %s.",
							   error != NULL && error->message != NULL ? error->message : "no details");
				}
		}
}

/**
 * zak_cgi_session_get_value_full:
 * @session:
 * @group:
 * @name:
 *
 * Returns: a value from session.
 */
gchar
*zak_cgi_session_get_value_full (ZakCgiSession *session, const gchar *group, const gchar *name)
{
	gchar *ret;

	GError *error;

	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	ret = NULL;
	if (priv->kfile != NULL)
		{
			error = NULL;
			ret = g_key_file_get_string (priv->kfile, group, name, &error);
			if (error != NULL)
				{
					g_warning ("Unable to get session value «%s» in group «%s»: %s",
							   name,
							   group,
							   error->message != NULL ? error->message : "no details.");
				}
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
	zak_cgi_session_set_value_full (session, "SESSION", name, value);
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
	return zak_cgi_session_get_value_full (session, "SESSION", name);
}

/**
 * zak_cgi_session_set_value_full_int:
 * @session:
 * @group:
 * @name:
 * @value:
 *
 */
void
zak_cgi_session_set_value_full_int (ZakCgiSession *session, const gchar *group, const gchar *name, gint value)
{
	gchar *str;

	str = g_strdup_printf ("%d", value);
	zak_cgi_session_set_value_full (session, group, name, str);
	g_free (str);
}

/**
 * zak_cgi_session_set_value_full_double:
 * @session:
 * @group:
 * @name:
 * @value:
 *
 */
void
zak_cgi_session_set_value_full_double (ZakCgiSession *session, const gchar *group, const gchar *name, gdouble value)
{
	gchar *str;

	str = g_strdup_printf ("%f", value);
	zak_cgi_session_set_value_full (session, group, name, str);
	g_free (str);
}

/**
 * zak_cgi_session_set_value_full_boolean:
 * @session:
 * @group:
 * @name:
 * @value:
 *
 */
void
zak_cgi_session_set_value_full_boolean (ZakCgiSession *session, const gchar *group, const gchar *name, gboolean value)
{
	zak_cgi_session_set_value_full_int (session, group, name, (gint)value);
}

/**
 * zak_cgi_session_get_value_full_int:
 * @session:
 * @group:
 * @name:
 *
 * Returns:
 */
gint
zak_cgi_session_get_value_full_int (ZakCgiSession *session, const gchar *group, const gchar *name)
{
	gchar *str;
	gint ret;

	str = zak_cgi_session_get_value_full (session, group, name);
	ret = strtol (str, NULL, 10);
	g_free (str);

	return ret;
}

/**
 * zak_cgi_session_get_value_full_double:
 * @session:
 * @group:
 * @name:
 *
 * Returns:
 */
gdouble
zak_cgi_session_get_value_full_double (ZakCgiSession *session, const gchar *group, const gchar *name)
{
	gchar *str;
	gdouble ret;

	str = zak_cgi_session_get_value_full (session, group, name);
	ret = g_strtod (str, NULL);
	g_free (str);

	return ret;
}

/**
 * zak_cgi_session_get_value_full_boolean:
 * @session:
 * @group:
 * @name:
 *
 * Returns:
 */
gboolean
zak_cgi_session_get_value_full_boolean (ZakCgiSession *session, const gchar *group, const gchar *name)
{
	return (gboolean)zak_cgi_session_get_value_full_int (session, group, name);
}

static ZakFormIniProvider
*zak_cgi_session_get_form_ini_provider (ZakCgiSession *session)
{
	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	if (priv->zakformini == NULL)
		{
			priv->zakformini = zak_form_ini_provider_new_from_gkeyfile (priv->kfile, g_file_get_path (priv->gfile));
		}

	return priv->zakformini;
}

/**
 * zak_cgi_session_set_from_form:
 * @session:
 * @form:
 *
 */
void
zak_cgi_session_set_from_form (ZakCgiSession *session, ZakFormForm *form)
{
	zak_form_form_insert (ZAK_FORM_FORM (form), ZAK_FORM_IPROVIDER (zak_cgi_session_get_form_ini_provider (session)));
}

/**
 * zak_cgi_session_fill_form:
 * @session:
 * @form:
 *
 */
void
zak_cgi_session_fill_form (ZakCgiSession *session, ZakFormForm *form)
{
	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	zak_cgi_session_get_form_ini_provider (session);
	zak_form_form_load (ZAK_FORM_FORM (form), ZAK_FORM_IPROVIDER (priv->zakformini));
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

	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	filename = g_build_filename (priv->path != NULL ? priv->path : g_get_tmp_dir (), priv->sid, NULL);

	return filename;
}

static void
zak_cgi_session_create_file (ZakCgiSession *session)
{
	gchar *filename;

	GError *error;
	GFileIOStream *iostream;

	guint32 i;
	gchar *tmp;

	GDateTime *gdt;

	ZakCgiSessionPrivate *priv = ZAK_CGI_SESSION_GET_PRIVATE (session);

	/* create new random name */
	i = g_random_int ();

	tmp = g_strdup_printf ("%d", i);

	priv->sid = g_compute_checksum_for_string (G_CHECKSUM_MD5,
											   tmp,
											   strlen (tmp));

	g_free (tmp);

	/* see if file already exists */
	filename = zak_cgi_session_build_filename (session);
	priv->gfile = g_file_new_for_path (filename);
	g_free (filename);

	error = NULL;
	iostream = g_file_replace_readwrite (priv->gfile, NULL, FALSE, G_FILE_CREATE_PRIVATE, NULL, &error);
	if (iostream == NULL
		|| error != NULL)
		{
			g_warning ("Unable to create the session file «%s»: %s.",
					   filename,
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
				}
			else
				{
					/* write REMOTE_ADDR and creation timestamp */
					gdt = g_date_time_new_now_local ();

					g_key_file_set_string (priv->kfile, "ZAKCGI", "REMOTE_ADDR", g_getenv ("REMOTE_ADDR"));
					g_key_file_set_string (priv->kfile, "ZAKCGI", "TIMESTAMP", g_date_time_format (gdt, "%FT%T"));
					g_key_file_save_to_file (priv->kfile, g_file_get_path (priv->gfile), NULL);

					g_date_time_unref (gdt);
				}
		}
}
