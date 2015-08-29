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

#include "commons.h"
#include "form.h"

static void zak_cgi_form_class_init (ZakCgiFormClass *class);
static void zak_cgi_form_init (ZakCgiForm *zak_cgi_form);

static void zak_cgi_form_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_cgi_form_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

static void zak_cgi_form_dispose (GObject *gobject);
static void zak_cgi_form_finalize (GObject *gobject);

#define ZAK_CGI_FORM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_CGI_TYPE_FORM, ZakCgiFormPrivate))

typedef struct _ZakCgiFormPrivate ZakCgiFormPrivate;
struct _ZakCgiFormPrivate
	{
		ZakCgiMain *zakcgimain;
		GHashTable *ht_attrs;
		guint elems;
		GHashTable *ht_elems;
	};

G_DEFINE_TYPE (ZakCgiForm, zak_cgi_form, G_TYPE_OBJECT)

static void
zak_cgi_form_class_init (ZakCgiFormClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = zak_cgi_form_set_property;
	object_class->get_property = zak_cgi_form_get_property;
	object_class->dispose = zak_cgi_form_dispose;
	object_class->finalize = zak_cgi_form_finalize;

	g_type_class_add_private (object_class, sizeof (ZakCgiFormPrivate));
}

static void
zak_cgi_form_init (ZakCgiForm *zak_cgi_form)
{
	ZakCgiFormPrivate *priv = ZAK_CGI_FORM_GET_PRIVATE (zak_cgi_form);

	priv->zakcgimain = NULL;
	priv->ht_attrs = NULL;
	priv->elems = 0;
	priv->ht_elems = g_hash_table_new (g_str_hash, g_str_equal);
}

/**
 * zak_cgi_form_new:
 * @zakcgimain:
 * @...:
 *
 * Returns: the newly created #ZakCgiForm object.
 */
ZakCgiForm
*zak_cgi_form_new (ZakCgiMain *zakcgimain, ...)
{
	ZakCgiForm *zak_cgi_form;
	ZakCgiFormPrivate *priv;

	va_list ap;

	zak_cgi_form = ZAK_CGI_FORM (g_object_new (zak_cgi_form_get_type (), NULL));

	priv = ZAK_CGI_FORM_GET_PRIVATE (zak_cgi_form);
	priv->zakcgimain = zakcgimain;

	va_start (ap, zakcgimain);
	priv->ht_attrs = zak_cgi_commons_valist_to_ghashtable (ap);

	return zak_cgi_form;
}

/**
 * zak_cgi_form_add_element:
 * @zakcgiform:
 * @element:
 *
 * Returns: #TRUE if @element is added; FALSE otherwise.
 */
gboolean
zak_cgi_form_add_element (ZakCgiForm *zakcgiform, ZakCgiFormElement *element)
{
	gboolean ret;
	gchar *id;

	ZakCgiFormPrivate *priv;

	priv = ZAK_CGI_FORM_GET_PRIVATE (zakcgiform);

	id = zak_cgi_form_element_get_id (element);

	if (g_hash_table_lookup (priv->ht_elems, id))
		{
			g_warning ("You cannot add an element with id already present in the form.");
			ret = FALSE;
		}
	else
		{
			priv->elems++;
			g_hash_table_insert (priv->ht_elems, g_strdup (id), g_object_ref (element));
			ret = TRUE;
		}

	g_free (id);

	return ret;
}

/**
 * zak_cgi_form_add_str:
 * @zakcgiform:
 * @str:
 *
 * Returns: #TRUE if @str is added; FALSE otherwise.
 */
gboolean
zak_cgi_form_add_str (ZakCgiForm *zakcgiform, const gchar *str)
{
	gboolean ret;
	gchar *id;

	ZakCgiFormPrivate *priv;

	priv = ZAK_CGI_FORM_GET_PRIVATE (zakcgiform);

	id = g_strdup_printf ("{id_%d}", priv->elems++);

	if (g_hash_table_lookup (priv->ht_elems, id))
		{
			priv->elems--;
			g_warning ("You cannot add an element with id already present in the form.");
			ret = FALSE;
		}
	else
		{
			g_hash_table_insert (priv->ht_elems, g_strdup (id), g_strdup (str));
			ret = TRUE;
		}

	g_free (id);

	return ret;
}

/**
 * zak_cgi_form_bind:
 * @zakcgiform:
 *
 */
void
zak_cgi_form_bind (ZakCgiForm *zakcgiform)
{
	GHashTableIter iter;
	gpointer key;
	gpointer value;

	GValue *gval;

	ZakCgiFormPrivate *priv;

	g_return_if_fail (ZAK_CGI_IS_FORM (zakcgiform));

	priv = ZAK_CGI_FORM_GET_PRIVATE (zakcgiform);

	g_hash_table_iter_init (&iter, priv->ht_elems);
	while (g_hash_table_iter_next (&iter, &key, &value))
		{
			if (!g_str_has_prefix ((gchar *)key, "{id_"))
				{
					gval = zak_cgi_main_get_stdin_field (priv->zakcgimain, (gchar *)key);
					if (gval != NULL)
						{
							zak_cgi_form_element_set_value ((ZakCgiFormElement *)value, gval);
						}
				}
		}
}

/**
 * zak_cgi_form_render_start:
 * @zakcgiform:
 *
 * Returns:
 */
gchar
*zak_cgi_form_render_start (ZakCgiForm *zakcgiform)
{
	gchar *ret;

	GString *str;

	ZakCgiFormPrivate *priv;

	g_return_if_fail (ZAK_CGI_IS_FORM (zakcgiform));

	priv = ZAK_CGI_FORM_GET_PRIVATE (zakcgiform);

	str = g_string_new ("<form");

	ret = zak_cgi_commons_ghashtable_to_str_attrs (priv->ht_attrs);
	g_string_append_printf (str, "%s>", ret);
	g_free (ret);

	ret = g_strdup (str->str);

	return ret;
}

/**
 * zak_cgi_form_render:
 * @zakcgiform:
 *
 * Returns:
 */
gchar
*zak_cgi_form_render (ZakCgiForm *zakcgiform)
{
	GString *str;

	GHashTableIter iter;
	gpointer key;
	gpointer value;

	gchar *tmp;

	ZakCgiFormPrivate *priv;

	priv = ZAK_CGI_FORM_GET_PRIVATE (zakcgiform);

	str = g_string_new ("");

	tmp = zak_cgi_form_render_start (zakcgiform);
	g_string_append (str, tmp);
	g_free (tmp);

	g_hash_table_iter_init (&iter, priv->ht_elems);
	while (g_hash_table_iter_next (&iter, &key, &value))
		{
			if (g_str_has_prefix ((gchar *)key, "{id_"))
				{
					g_string_append_printf (str, "\n%s", (gchar *)value);
				}
			else
				{
					tmp = zak_cgi_form_element_render ((ZakCgiFormElement *)value);
					g_string_append_printf (str, "\n%s", tmp);
					g_free (tmp);
				}
		}

	g_string_append (str, "\n</form>");

	return g_strdup (str->str);
}

/* PRIVATE */
static void
zak_cgi_form_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiForm *zak_cgi_form = (ZakCgiForm *)object;
	ZakCgiFormPrivate *priv = ZAK_CGI_FORM_GET_PRIVATE (zak_cgi_form);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiForm *zak_cgi_form = (ZakCgiForm *)object;
	ZakCgiFormPrivate *priv = ZAK_CGI_FORM_GET_PRIVATE (zak_cgi_form);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_dispose (GObject *gobject)
{
	ZakCgiForm *zak_cgi_form = (ZakCgiForm *)gobject;
	ZakCgiFormPrivate *priv = ZAK_CGI_FORM_GET_PRIVATE (zak_cgi_form);


	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->dispose (gobject);
}

static void
zak_cgi_form_finalize (GObject *gobject)
{
	ZakCgiForm *zak_cgi_form = (ZakCgiForm *)gobject;
	ZakCgiFormPrivate *priv = ZAK_CGI_FORM_GET_PRIVATE (zak_cgi_form);


	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->finalize (gobject);
}
