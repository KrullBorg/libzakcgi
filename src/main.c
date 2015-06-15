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

#include <unistd.h>

#include "main.h"

extern char **environ;

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

	guint l;
	guint i;
	gchar **envs;

	ht = g_hash_table_new (g_str_hash, g_str_equal);

	l = g_strv_length (environ);
	for (i = 0; i < l; i++)
		{
			envs = g_strsplit (environ[i], "=", 2);
			g_hash_table_replace (ht, g_strdup (envs[0]), g_strdup (envs[1]));
			g_strfreev (envs);
		}

	return ht;
}

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
