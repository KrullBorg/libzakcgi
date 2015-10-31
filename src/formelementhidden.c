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
#include "tag.h"
#include "formelementhidden.h"

static void zak_cgi_form_element_hidden_class_init (ZakCgiFormElementHiddenClass *class);
static void zak_cgi_form_element_hidden_init (ZakCgiFormElementHidden *zak_cgi_form_element_hidden);

static gchar *zak_cgi_form_element_hidden_render (ZakCgiFormElement *element);

static void zak_cgi_form_element_hidden_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_cgi_form_element_hidden_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

static void zak_cgi_form_element_hidden_dispose (GObject *gobject);
static void zak_cgi_form_element_hidden_finalize (GObject *gobject);

#define ZAK_CGI_FORM_ELEMENT_HIDDEN_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_CGI_TYPE_FORM_ELEMENT_HIDDEN, ZakCgiFormElementHiddenPrivate))

typedef struct _ZakCgiFormElementHiddenPrivate ZakCgiFormElementHiddenPrivate;
struct _ZakCgiFormElementHiddenPrivate
	{
		gpointer empty;
	};

G_DEFINE_TYPE (ZakCgiFormElementHidden, zak_cgi_form_element_hidden, ZAK_CGI_TYPE_FORM_ELEMENT)

static void
zak_cgi_form_element_hidden_class_init (ZakCgiFormElementHiddenClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	ZakCgiFormElementClass *elem_class = ZAK_CGI_FORM_ELEMENT_CLASS (klass);

	object_class->set_property = zak_cgi_form_element_hidden_set_property;
	object_class->get_property = zak_cgi_form_element_hidden_get_property;
	object_class->dispose = zak_cgi_form_element_hidden_dispose;
	object_class->finalize = zak_cgi_form_element_hidden_finalize;

	elem_class->render = zak_cgi_form_element_hidden_render;

	g_type_class_add_private (object_class, sizeof (ZakCgiFormElementHiddenPrivate));
}

static void
zak_cgi_form_element_hidden_init (ZakCgiFormElementHidden *zak_cgi_form_element_hidden)
{
	ZakCgiFormElementHiddenPrivate *priv = ZAK_CGI_FORM_ELEMENT_HIDDEN_GET_PRIVATE (zak_cgi_form_element_hidden);

}

/**
 * zak_cgi_form_element_hidden_new:
 * @id:
 * @...:
 *
 * Returns: the newly created #ZakCgiFormElementHidden object.
 */
ZakCgiFormElement
*zak_cgi_form_element_hidden_new (const gchar *id,
								  ...)
{
	va_list ap;

	GHashTable *ht_attrs;

	ZakCgiFormElementHidden *zak_cgi_form_element_hidden;

	zak_cgi_form_element_hidden = ZAK_CGI_FORM_ELEMENT_HIDDEN (g_object_new (zak_cgi_form_element_hidden_get_type (), NULL));

	va_start (ap, id);

	ht_attrs = zak_cgi_commons_valist_to_ghashtable (ap);
	g_hash_table_replace (ht_attrs, "type", "hidden");

	ZAK_CGI_FORM_ELEMENT_CLASS (zak_cgi_form_element_hidden_parent_class)->construct (ZAK_CGI_FORM_ELEMENT (zak_cgi_form_element_hidden),
																					id,
																					ht_attrs);

	return ZAK_CGI_FORM_ELEMENT (zak_cgi_form_element_hidden);
}

static gchar
*zak_cgi_form_element_hidden_render (ZakCgiFormElement *element)
{
	gchar *ret;

	GHashTable *ht_attrs;

	gchar *value;

	ZakCgiFormElementClass *klass;

	klass = (ZakCgiFormElementClass *)g_type_class_peek_parent (ZAK_CGI_FORM_ELEMENT_HIDDEN_GET_CLASS (ZAK_CGI_FORM_ELEMENT_HIDDEN (element)));

	ht_attrs = klass->get_ht_attrs (element);

	value = zak_form_element_get_value (ZAK_FORM_ELEMENT (element));
	if (value != NULL)
		{
			g_hash_table_insert (ht_attrs, (gpointer)"value", (gpointer)g_strdup (value));
		}

	ret = zak_cgi_tag_tag_ht ("input", zak_cgi_form_element_get_id (element), ht_attrs);

	return ret;
}

/* PRIVATE */
static void
zak_cgi_form_element_hidden_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiFormElementHidden *zak_cgi_form_element_hidden = (ZakCgiFormElementHidden *)object;
	ZakCgiFormElementHiddenPrivate *priv = ZAK_CGI_FORM_ELEMENT_HIDDEN_GET_PRIVATE (zak_cgi_form_element_hidden);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_element_hidden_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiFormElementHidden *zak_cgi_form_element_hidden = (ZakCgiFormElementHidden *)object;
	ZakCgiFormElementHiddenPrivate *priv = ZAK_CGI_FORM_ELEMENT_HIDDEN_GET_PRIVATE (zak_cgi_form_element_hidden);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_element_hidden_dispose (GObject *gobject)
{
	ZakCgiFormElementHidden *zak_cgi_form_element_hidden = (ZakCgiFormElementHidden *)gobject;
	ZakCgiFormElementHiddenPrivate *priv = ZAK_CGI_FORM_ELEMENT_HIDDEN_GET_PRIVATE (zak_cgi_form_element_hidden);



	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->dispose (gobject);
}

static void
zak_cgi_form_element_hidden_finalize (GObject *gobject)
{
	ZakCgiFormElementHidden *zak_cgi_form_element_hidden = (ZakCgiFormElementHidden *)gobject;
	ZakCgiFormElementHiddenPrivate *priv = ZAK_CGI_FORM_ELEMENT_HIDDEN_GET_PRIVATE (zak_cgi_form_element_hidden);



	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->finalize (gobject);
}
