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
#include "formelementtext.h"

static void zak_cgi_form_element_text_class_init (ZakCgiFormElementTextClass *class);
static void zak_cgi_form_element_text_init (ZakCgiFormElementText *zak_cgi_form_element_text);

static gchar *zak_cgi_form_element_text_render (ZakCgiFormElement *element);

static gboolean zak_cgi_form_element_text_is_valid (ZakCgiFormElement *element);

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
		gpointer empty;
	};

G_DEFINE_TYPE (ZakCgiFormElementText, zak_cgi_form_element_text, ZAK_CGI_TYPE_FORM_ELEMENT)

static void
zak_cgi_form_element_text_class_init (ZakCgiFormElementTextClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	ZakCgiFormElementClass *elem_class = ZAK_CGI_FORM_ELEMENT_CLASS (klass);

	object_class->set_property = zak_cgi_form_element_text_set_property;
	object_class->get_property = zak_cgi_form_element_text_get_property;
	object_class->dispose = zak_cgi_form_element_text_dispose;
	object_class->finalize = zak_cgi_form_element_text_finalize;

	elem_class->render = zak_cgi_form_element_text_render;
	elem_class->is_valid = zak_cgi_form_element_text_is_valid;

	g_type_class_add_private (object_class, sizeof (ZakCgiFormElementTextPrivate));
}

static void
zak_cgi_form_element_text_init (ZakCgiFormElementText *zak_cgi_form_element_text)
{
	ZakCgiFormElementTextPrivate *priv = ZAK_CGI_FORM_ELEMENT_TEXT_GET_PRIVATE (zak_cgi_form_element_text);

}

/**
 * zak_cgi_form_element_text_new:
 * @id:
 * @validation_regex:
 * @...:
 *
 * Returns: the newly created #ZakCgiFormElementText object.
 */
ZakCgiFormElement
*zak_cgi_form_element_text_new (const gchar *id,
								const gchar *validation_regex,
								...)
{
	va_list ap;

	ZakCgiFormElementText *zak_cgi_form_element_text;

	zak_cgi_form_element_text = ZAK_CGI_FORM_ELEMENT_TEXT (g_object_new (zak_cgi_form_element_text_get_type (), NULL));

	va_start (ap, validation_regex);

	ZAK_CGI_FORM_ELEMENT_CLASS (zak_cgi_form_element_text_parent_class)->construct (ZAK_CGI_FORM_ELEMENT (zak_cgi_form_element_text),
																					id,
																					validation_regex,
																					zak_cgi_commons_valist_to_ghashtable (ap));

	return ZAK_CGI_FORM_ELEMENT (zak_cgi_form_element_text);
}

static gchar
*zak_cgi_form_element_text_render (ZakCgiFormElement *element)
{
	gchar *ret;

	GHashTable *ht_attrs;

	GValue *value;

	ZakCgiFormElementClass *klass;

	klass = (ZakCgiFormElementClass *)g_type_class_peek_parent (ZAK_CGI_FORM_ELEMENT_TEXT_GET_CLASS (ZAK_CGI_FORM_ELEMENT_TEXT (element)));

	ht_attrs = klass->get_ht_attrs (element);

	value = zak_cgi_form_element_get_value (element);
	if (value != NULL)
		{
			g_hash_table_insert (ht_attrs, (gpointer)"value", (gpointer)g_value_get_string (value));
		}

	ret = zak_cgi_tag_text_ht (zak_cgi_form_element_get_id (element), ht_attrs);

	return ret;
}

static gboolean
zak_cgi_form_element_text_check_value (const gchar *validation_regex, GValue *value)
{
	gboolean ret;

	GRegex *regex;
	GError *error;

	if (G_VALUE_HOLDS (value, G_TYPE_STRING))
		{
			error = NULL;
			regex = g_regex_new (validation_regex, 0, 0, &error);
			if (regex == NULL
				|| error != NULL)
				{
					syslog (LOG_MAKEPRI(LOG_SYSLOG, LOG_DEBUG), "Error on creating regex: %s.",
							error->message != NULL ? error->message : "no details");
					return FALSE;
				}

			ret = g_regex_match ((const GRegex *)regex, g_value_get_string (value), 0, NULL);
		}
	else
		{
			ret = FALSE;
		}

	return ret;
}

static gboolean
zak_cgi_form_element_text_is_valid (ZakCgiFormElement *element)
{
	gboolean ret;

	GValue *gval;

	gchar *str_regex;

	gval = zak_cgi_form_element_get_value (element);

	g_object_get (G_OBJECT (element),
				  "validation-regex", &str_regex,
				  NULL);

	if (G_VALUE_HOLDS (gval, G_TYPE_PTR_ARRAY))
		{
			guint i;
			GPtrArray *ar = (GPtrArray *)g_value_get_boxed (gval);
			for (i = 0; i < ar->len; i++)
				{
					if (!zak_cgi_form_element_text_check_value (str_regex, (GValue *)g_ptr_array_index (ar, i)))
						{
							ret = FALSE;
							break;
						}
				}
		}
	else
		{
			ret = zak_cgi_form_element_text_check_value (str_regex, gval);
		}

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
