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

#include <syslog.h>

#include "main.h"
#include "url.h"

static void zak_cgi_url_class_init (ZakCgiUrlClass *class);
static void zak_cgi_url_init (ZakCgiUrl *zak_cgi_url);

static void zak_cgi_url_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_cgi_url_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

static void zak_cgi_url_dispose (GObject *gobject);
static void zak_cgi_url_finalize (GObject *gobject);

#define ZAK_CGI_URL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_CGI_TYPE_URL, ZakCgiUrlPrivate))

typedef struct _ZakCgiUrlPrivate ZakCgiUrlPrivate;
struct _ZakCgiUrlPrivate
	{
		gchar *controller;
		gchar *action;

		GHashTable *ht_functions;
	};

G_DEFINE_TYPE (ZakCgiUrl, zak_cgi_url, G_TYPE_OBJECT)

static void
zak_cgi_url_class_init (ZakCgiUrlClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = zak_cgi_url_set_property;
	object_class->get_property = zak_cgi_url_get_property;
	object_class->dispose = zak_cgi_url_dispose;
	object_class->finalize = zak_cgi_url_finalize;

	g_type_class_add_private (object_class, sizeof (ZakCgiUrlPrivate));
}

static void
zak_cgi_url_init (ZakCgiUrl *zak_cgi_url)
{
	ZakCgiUrlPrivate *priv = ZAK_CGI_URL_GET_PRIVATE (zak_cgi_url);

	priv->controller = NULL;
	priv->action = NULL;

	priv->ht_functions = g_hash_table_new (g_str_hash, g_str_equal);
}

/**
 * zak_cgi_url_new:
 *
 * Returns: the newly created #ZakCgiUrl object.
 */
ZakCgiUrl
*zak_cgi_url_new (void)
{
	ZakCgiUrl *zak_cgi_url;
	ZakCgiUrlPrivate *priv;

	GHashTable *ht_env;

	GValue *url;

	zak_cgi_url = ZAK_CGI_URL (g_object_new (zak_cgi_url_get_type (), NULL));

	priv = ZAK_CGI_URL_GET_PRIVATE (zak_cgi_url);

	/* parsing */
	ht_env = zak_cgi_main_get_parameters (NULL);
	url = g_hash_table_lookup (ht_env, "_url");
	if (url != NULL)
		{
			gchar **splitted;

			splitted = g_strsplit (g_value_get_string (url), "/", -1);
			if (g_strv_length (splitted) >= 3)
				{
					priv->controller = g_strdup (splitted[1]);
					priv->action = g_strdup (splitted [2]);
				}
			g_strfreev (splitted);
		}

	return zak_cgi_url;
}

void
zak_cgi_url_connect (ZakCgiUrl *url,
					 const gchar *controller,
					 const gchar *action,
					 ZakCgiUrlConnectedFunction function,
					 gpointer user_data)
{
	GPtrArray *ar;

	ZakCgiUrlPrivate *priv = ZAK_CGI_URL_GET_PRIVATE (url);

	ar = g_ptr_array_new ();
	g_ptr_array_add (ar, function);
	g_ptr_array_add (ar, user_data);

	g_hash_table_replace (priv->ht_functions, g_strdup_printf ("%s|%s", controller, action), g_ptr_array_ref (ar));

	g_ptr_array_unref (ar);
}

void
zak_cgi_url_dispatch (ZakCgiUrl *url)
{
	gchar *name;
	GPtrArray *ar;
	ZakCgiUrlConnectedFunction function;
	
	ZakCgiUrlPrivate *priv = ZAK_CGI_URL_GET_PRIVATE (url);

	name = g_strdup_printf ("%s|%s", priv->controller, priv->action);
	ar = (GPtrArray *)g_hash_table_lookup (priv->ht_functions, name);
	if (ar != NULL)
		{
			function = g_ptr_array_index (ar, 0);
			(*function)(g_ptr_array_index (ar, 1));
		}
}

/* PRIVATE */
static void
zak_cgi_url_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiUrl *zak_cgi_url = (ZakCgiUrl *)object;
	ZakCgiUrlPrivate *priv = ZAK_CGI_URL_GET_PRIVATE (zak_cgi_url);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_url_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiUrl *zak_cgi_url = (ZakCgiUrl *)object;
	ZakCgiUrlPrivate *priv = ZAK_CGI_URL_GET_PRIVATE (zak_cgi_url);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_url_dispose (GObject *gobject)
{
	ZakCgiUrl *zak_cgi_url = (ZakCgiUrl *)gobject;
	ZakCgiUrlPrivate *priv = ZAK_CGI_URL_GET_PRIVATE (zak_cgi_url);


	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->dispose (gobject);
}

static void
zak_cgi_url_finalize (GObject *gobject)
{
	ZakCgiUrl *zak_cgi_url = (ZakCgiUrl *)gobject;
	ZakCgiUrlPrivate *priv = ZAK_CGI_URL_GET_PRIVATE (zak_cgi_url);


	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->finalize (gobject);
}
