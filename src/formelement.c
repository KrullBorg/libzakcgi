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

#include "formelement.h"

static void zak_cgi_form_element_class_init (ZakCgiFormElementClass *class);
static void zak_cgi_form_element_init (ZakCgiFormElement *zak_cgi_form_element);

static void zak_cgi_form_element_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_cgi_form_element_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

static void zak_cgi_form_element_dispose (GObject *gobject);
static void zak_cgi_form_element_finalize (GObject *gobject);

#define ZAK_CGI_FORM_ELEMENT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_CGI_TYPE_FORM_ELEMENT, ZakCgiFormElementPrivate))

typedef struct _ZakCgiFormElementPrivate ZakCgiFormElementPrivate;
struct _ZakCgiFormElementPrivate
	{
	};

G_DEFINE_TYPE (ZakCgiFormElement, zak_cgi_form_element, G_TYPE_OBJECT)

static void
zak_cgi_form_element_class_init (ZakCgiFormElementClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = zak_cgi_form_element_set_property;
	object_class->get_property = zak_cgi_form_element_get_property;
	object_class->dispose = zak_cgi_form_element_dispose;
	object_class->finalize = zak_cgi_form_element_finalize;

	g_type_class_add_private (object_class, sizeof (ZakCgiFormElementPrivate));
}

static void
zak_cgi_form_element_init (ZakCgiFormElement *zak_cgi_form_element)
{
	ZakCgiFormElementPrivate *priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (zak_cgi_form_element);


}

/**
 * zak_cgi_form_element_new:
 *
 * Returns: the newly created #ZakCgiFormElement object.
 */
ZakCgiFormElement
*zak_cgi_form_element_new (void)
{
	ZakCgiFormElement *zak_cgi_form_element;
	ZakCgiFormElementPrivate *priv;

	zak_cgi_form_element = ZAK_CGI_FORM_ELEMENT (g_object_new (zak_cgi_form_element_get_type (), NULL));

	priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (zak_cgi_form_element);

	return zak_cgi_form_element;
}

/* PRIVATE */
static void
zak_cgi_form_element_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiFormElement *zak_cgi_form_element = (ZakCgiFormElement *)object;
	ZakCgiFormElementPrivate *priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (zak_cgi_form_element);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_element_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiFormElement *zak_cgi_form_element = (ZakCgiFormElement *)object;
	ZakCgiFormElementPrivate *priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (zak_cgi_form_element);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_element_dispose (GObject *gobject)
{
	ZakCgiFormElement *zak_cgi_form_element = (ZakCgiFormElement *)gobject;
	ZakCgiFormElementPrivate *priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (zak_cgi_form_element);


	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->dispose (gobject);
}

static void
zak_cgi_form_element_finalize (GObject *gobject)
{
	ZakCgiFormElement *zak_cgi_form_element = (ZakCgiFormElement *)gobject;
	ZakCgiFormElementPrivate *priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (zak_cgi_form_element);



	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->finalize (gobject);
}
