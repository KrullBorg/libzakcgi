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

enum
{
	PROP_0,
	PROP_VALIDATION_REGEX
};

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
		gchar *validation_regex;
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

	g_object_class_install_property (object_class, PROP_VALIDATION_REGEX,
	                                 g_param_spec_string ("validation-regex",
	                                                      "Validation regex",
	                                                      "Validation regex",
	                                                      "",
	                                                      G_PARAM_READWRITE));
}

static void
zak_cgi_form_element_init (ZakCgiFormElement *zak_cgi_form_element)
{
	ZakCgiFormElementPrivate *priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (zak_cgi_form_element);

	priv->validation_regex = NULL;
}

/**
 * zak_cgi_form_element_new:
 * @validation_regex:
 *
 * Returns: the newly created #ZakCgiFormElement object.
 */
ZakCgiFormElement
*zak_cgi_form_element_new (const gchar *validation_regex)
{
	ZakCgiFormElement *zak_cgi_form_element;
	ZakCgiFormElementPrivate *priv;

	zak_cgi_form_element = ZAK_CGI_FORM_ELEMENT (g_object_new (zak_cgi_form_element_get_type (), NULL));

	priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (zak_cgi_form_element);

	zak_cgi_form_element_set_validation_regex (zak_cgi_form_element, validation_regex);

	return zak_cgi_form_element;
}

void
zak_cgi_form_element_set_validation_regex (ZakCgiFormElement *element, const gchar *validation_regex)
{
	ZakCgiFormElementPrivate *priv;

	priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (element);

	if (validation_regex == NULL)
		{
			if (priv->validation_regex != NULL)
				{
					g_free (priv->validation_regex);
				}
			priv->validation_regex = NULL;
		}
	else
		{
			priv->validation_regex = g_strdup (validation_regex);
		}
}

gchar
*zak_cgi_form_element_get_validation_regex (ZakCgiFormElement *element)
{
	gchar *ret;

	ZakCgiFormElementPrivate *priv;

	priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (element);

	if (priv->validation_regex != NULL)
		{
			ret = g_strdup (priv->validation_regex);
		}
	else
		{
			ret = NULL;
		}

	return ret;
}

/**
 * zak_cgi_form_element_render:
 * @element:
 *
 */
gchar
*zak_cgi_form_element_render (ZakCgiFormElement *element)
{
	gchar *ret;

	ret = g_strdup ("");

	if (ZAK_CGI_IS_FORM_ELEMENT (element) && ZAK_CGI_FORM_ELEMENT_GET_CLASS (element)->render != NULL)
		{
			ret = ZAK_CGI_FORM_ELEMENT_CLASS (element)->render (element);
		}

	return ret;
}

/**
 * zak_cgi_form_element_is_valid:
 * @element:
 *
 */
gboolean
zak_cgi_form_element_is_valid (ZakCgiFormElement *element)
{
	gboolean ret;

	ret = FALSE;

	if (ZAK_CGI_IS_FORM_ELEMENT (element) && ZAK_CGI_FORM_ELEMENT_GET_CLASS (element)->is_valid != NULL)
		{
			ret = ZAK_CGI_FORM_ELEMENT_CLASS (element)->is_valid (element);
		}

	return ret;
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
			case PROP_VALIDATION_REGEX:
				zak_cgi_form_element_set_validation_regex (zak_cgi_form_element, g_value_get_string (value));
				break;

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
			case PROP_VALIDATION_REGEX:
				g_value_set_string (value, zak_cgi_form_element_get_validation_regex (zak_cgi_form_element));
				break;

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
