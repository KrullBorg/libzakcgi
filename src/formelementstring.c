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
#include "formelementstring.h"

static void zak_cgi_form_element_string_class_init (ZakCgiFormElementStringClass *class);
static void zak_cgi_form_element_string_init (ZakCgiFormElementString *zak_cgi_form_element_string);

static gchar *zak_cgi_form_element_string_render (ZakCgiFormElement *element);

static gboolean zak_cgi_form_element_string_is_valid (ZakCgiFormElement *element);

static void zak_cgi_form_element_string_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_cgi_form_element_string_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

static void zak_cgi_form_element_string_dispose (GObject *gobject);
static void zak_cgi_form_element_string_finalize (GObject *gobject);

#define ZAK_CGI_FORM_ELEMENT_STRING_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_CGI_TYPE_FORM_ELEMENT_STRING, ZakCgiFormElementStringPrivate))

typedef struct _ZakCgiFormElementStringPrivate ZakCgiFormElementStringPrivate;
struct _ZakCgiFormElementStringPrivate
	{
		gchar *str;
	};

G_DEFINE_TYPE (ZakCgiFormElementString, zak_cgi_form_element_string, ZAK_CGI_TYPE_FORM_ELEMENT)

static void
zak_cgi_form_element_string_class_init (ZakCgiFormElementStringClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	ZakCgiFormElementClass *elem_class = ZAK_CGI_FORM_ELEMENT_CLASS (klass);

	object_class->set_property = zak_cgi_form_element_string_set_property;
	object_class->get_property = zak_cgi_form_element_string_get_property;
	object_class->dispose = zak_cgi_form_element_string_dispose;
	object_class->finalize = zak_cgi_form_element_string_finalize;

	elem_class->render = zak_cgi_form_element_string_render;
	elem_class->is_valid = zak_cgi_form_element_string_is_valid;

	g_type_class_add_private (object_class, sizeof (ZakCgiFormElementStringPrivate));
}

static void
zak_cgi_form_element_string_init (ZakCgiFormElementString *zak_cgi_form_element_string)
{
	ZakCgiFormElementStringPrivate *priv = ZAK_CGI_FORM_ELEMENT_STRING_GET_PRIVATE (zak_cgi_form_element_string);

	priv->str = NULL;
}

/**
 * zak_cgi_form_element_string_new:
 * @str:
 *
 * Returns: the newly created #ZakCgiFormElementString object.
 */
ZakCgiFormElement
*zak_cgi_form_element_string_new (const gchar *str)
{
	ZakCgiFormElementString *zak_cgi_form_element_string;

	zak_cgi_form_element_string = ZAK_CGI_FORM_ELEMENT_STRING (g_object_new (zak_cgi_form_element_string_get_type (), NULL));

	ZakCgiFormElementStringPrivate *priv = ZAK_CGI_FORM_ELEMENT_STRING_GET_PRIVATE (zak_cgi_form_element_string);

	priv->str = g_strdup (str);

	return ZAK_CGI_FORM_ELEMENT (zak_cgi_form_element_string);
}

static gchar
*zak_cgi_form_element_string_render (ZakCgiFormElement *element)
{
	gchar *ret;

	ZakCgiFormElementStringPrivate *priv = ZAK_CGI_FORM_ELEMENT_STRING_GET_PRIVATE (element);

	ret = g_strdup (priv->str);

	return ret;
}

static gboolean
zak_cgi_form_element_string_check_value (const gchar *validation_regex, GValue *value)
{
	return TRUE;
}

static gboolean
zak_cgi_form_element_string_is_valid (ZakCgiFormElement *element)
{
	return TRUE;
}

/* PRIVATE */
static void
zak_cgi_form_element_string_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiFormElementString *zak_cgi_form_element_string = (ZakCgiFormElementString *)object;
	ZakCgiFormElementStringPrivate *priv = ZAK_CGI_FORM_ELEMENT_STRING_GET_PRIVATE (zak_cgi_form_element_string);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_element_string_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiFormElementString *zak_cgi_form_element_string = (ZakCgiFormElementString *)object;
	ZakCgiFormElementStringPrivate *priv = ZAK_CGI_FORM_ELEMENT_STRING_GET_PRIVATE (zak_cgi_form_element_string);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_element_string_dispose (GObject *gobject)
{
	ZakCgiFormElementString *zak_cgi_form_element_string = (ZakCgiFormElementString *)gobject;
	ZakCgiFormElementStringPrivate *priv = ZAK_CGI_FORM_ELEMENT_STRING_GET_PRIVATE (zak_cgi_form_element_string);



	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->dispose (gobject);
}

static void
zak_cgi_form_element_string_finalize (GObject *gobject)
{
	ZakCgiFormElementString *zak_cgi_form_element_string = (ZakCgiFormElementString *)gobject;
	ZakCgiFormElementStringPrivate *priv = ZAK_CGI_FORM_ELEMENT_STRING_GET_PRIVATE (zak_cgi_form_element_string);



	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->finalize (gobject);
}
