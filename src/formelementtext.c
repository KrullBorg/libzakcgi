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

#include "formelementtext.h"

static void zak_cgi_form_element_text_class_init (ZakCgiFormElementTextClass *class);
static void zak_cgi_form_element_text_init (ZakCgiFormElementText *zak_cgi_form_element_text);

static void zak_cgi_form_element_text_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_cgi_form_element_text_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

static void zak_cgi_form_element_text_dispose (GObject *gobject);
static void zak_cgi_form_element_text_finalize (GObject *gobject);

#define ZAK_CGI_FORM_ELEMENT_TEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_CGI_TYPE_FORM_ELEMENT_TEXT, ZakCgiFormElementTextPrivate))

typedef struct _ZakCgiFormElementTextPrivate ZakCgiFormElementTextPrivate;
struct _ZakCgiFormElementTextPrivate
	{
	};

G_DEFINE_TYPE (ZakCgiFormElementText, zak_cgi_form_element_text, ZAK_CGI_TYPE_FORM_ELEMENT)

static void
zak_cgi_form_element_text_class_init (ZakCgiFormElementTextClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = zak_cgi_form_element_text_set_property;
	object_class->get_property = zak_cgi_form_element_text_get_property;
	object_class->dispose = zak_cgi_form_element_text_dispose;
	object_class->finalize = zak_cgi_form_element_text_finalize;

	g_type_class_add_private (object_class, sizeof (ZakCgiFormElementTextPrivate));
}

static void
zak_cgi_form_element_text_init (ZakCgiFormElementText *zak_cgi_form_element_text)
{
	ZakCgiFormElementTextPrivate *priv = ZAK_CGI_FORM_ELEMENT_TEXT_GET_PRIVATE (zak_cgi_form_element_text);

}

/**
 * zak_cgi_form_element_text_new:
 * @validation_regex:
 *
 * Returns: the newly created #ZakCgiFormElementText object.
 */
ZakCgiFormElementText
*zak_cgi_form_element_text_new (const gchar *validation_regex)
{
	ZakCgiFormElementText *zak_cgi_form_element_text;
	ZakCgiFormElementTextPrivate *priv;

	zak_cgi_form_element_text = ZAK_CGI_FORM_ELEMENT_TEXT (g_object_new (zak_cgi_form_element_text_get_type (), NULL));

	priv = ZAK_CGI_FORM_ELEMENT_TEXT_GET_PRIVATE (zak_cgi_form_element_text);

	if (validation_regex != NULL)
		{
		}

	return zak_cgi_form_element_text;
}

/**
 * zak_cgi_form_element_text_render:
 * @element_text:
 *
 */
gchar
*zak_cgi_form_element_text_render (ZakCgiFormElementText *element_text)
{
	gchar *ret;


	return ret;
}

/**
 * zak_cgi_form_element_text_is_valid:
 * @element_text:
 *
 */
gboolean
zak_cgi_form_element_text_is_valid (ZakCgiFormElementText *element_text)
{
	gboolean ret;


	return ret;
}

/* PRIVATE */
static void
zak_cgi_form_element_text_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiFormElementText *zak_cgi_form_element_text = (ZakCgiFormElementText *)object;
	ZakCgiFormElementTextPrivate *priv = ZAK_CGI_FORM_ELEMENT_TEXT_GET_PRIVATE (zak_cgi_form_element_text);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_element_text_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiFormElementText *zak_cgi_form_element_text = (ZakCgiFormElementText *)object;
	ZakCgiFormElementTextPrivate *priv = ZAK_CGI_FORM_ELEMENT_TEXT_GET_PRIVATE (zak_cgi_form_element_text);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_element_text_dispose (GObject *gobject)
{
	ZakCgiFormElementText *zak_cgi_form_element_text = (ZakCgiFormElementText *)gobject;
	ZakCgiFormElementTextPrivate *priv = ZAK_CGI_FORM_ELEMENT_TEXT_GET_PRIVATE (zak_cgi_form_element_text);



	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->dispose (gobject);
}

static void
zak_cgi_form_element_text_finalize (GObject *gobject)
{
	ZakCgiFormElementText *zak_cgi_form_element_text = (ZakCgiFormElementText *)gobject;
	ZakCgiFormElementTextPrivate *priv = ZAK_CGI_FORM_ELEMENT_TEXT_GET_PRIVATE (zak_cgi_form_element_text);



	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->finalize (gobject);
}
