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
#include "formelementselect.h"

static void zak_cgi_form_element_select_class_init (ZakCgiFormElementSelectClass *class);
static void zak_cgi_form_element_select_init (ZakCgiFormElementSelect *zak_cgi_form_element_select);

static gchar *zak_cgi_form_element_select_render (ZakCgiFormElement *element);

static gboolean zak_cgi_form_element_select_is_valid (ZakCgiFormElement *element);

static void zak_cgi_form_element_select_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_cgi_form_element_select_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

static void zak_cgi_form_element_select_dispose (GObject *gobject);
static void zak_cgi_form_element_select_finalize (GObject *gobject);

#define ZAK_CGI_FORM_ELEMENT_SELECT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_CGI_TYPE_FORM_ELEMENT_SELECT, ZakCgiFormElementSelectPrivate))

typedef struct _ZakCgiFormElementSelectPrivate ZakCgiFormElementSelectPrivate;
struct _ZakCgiFormElementSelectPrivate
	{
		GHashTable *ht_options;
	};

G_DEFINE_TYPE (ZakCgiFormElementSelect, zak_cgi_form_element_select, ZAK_CGI_TYPE_FORM_ELEMENT)

static void
zak_cgi_form_element_select_class_init (ZakCgiFormElementSelectClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	ZakCgiFormElementClass *elem_class = ZAK_CGI_FORM_ELEMENT_CLASS (klass);

	object_class->set_property = zak_cgi_form_element_select_set_property;
	object_class->get_property = zak_cgi_form_element_select_get_property;
	object_class->dispose = zak_cgi_form_element_select_dispose;
	object_class->finalize = zak_cgi_form_element_select_finalize;

	elem_class->render = zak_cgi_form_element_select_render;
	elem_class->is_valid = zak_cgi_form_element_select_is_valid;

	g_type_class_add_private (object_class, sizeof (ZakCgiFormElementSelectPrivate));
}

static void
zak_cgi_form_element_select_init (ZakCgiFormElementSelect *zak_cgi_form_element_select)
{
	ZakCgiFormElementSelectPrivate *priv = ZAK_CGI_FORM_ELEMENT_SELECT_GET_PRIVATE (zak_cgi_form_element_select);

	priv->ht_options = g_hash_table_new (g_str_hash, g_str_equal);
}

/**
 * zak_cgi_form_element_select_new:
 * @id:
 * @validation_regex:
 * @...:
 *
 * Returns: the newly created #ZakCgiFormElementSelect object.
 */
ZakCgiFormElement
*zak_cgi_form_element_select_new (const gchar *id,
								  const gchar *validation_regex,
								  ...)
{
	va_list ap;

	ZakCgiFormElementSelect *zak_cgi_form_element_select;

	zak_cgi_form_element_select = ZAK_CGI_FORM_ELEMENT_SELECT (g_object_new (zak_cgi_form_element_select_get_type (), NULL));

	va_start (ap, validation_regex);

	ZAK_CGI_FORM_ELEMENT_CLASS (zak_cgi_form_element_select_parent_class)->construct (ZAK_CGI_FORM_ELEMENT (zak_cgi_form_element_select),
																					id,
																					validation_regex,
																					zak_cgi_commons_valist_to_ghashtable (ap));

	return ZAK_CGI_FORM_ELEMENT (zak_cgi_form_element_select);
}

/**
 * zak_cgi_form_element_select_add_option:
 * @element:
 * @value:
 * @content:
 * @...:
 *
 */
void
zak_cgi_form_element_select_add_option (ZakCgiFormElementSelect *element,
										const gchar *value,
										const gchar *content,
										...)
{
	va_list ap;
	GHashTable *ht_attrs;

	ZakCgiFormElementSelectPrivate *priv = ZAK_CGI_FORM_ELEMENT_SELECT_GET_PRIVATE (element);

	va_start (ap, content);
	ht_attrs = zak_cgi_commons_valist_to_ghashtable (ap);
	if (ht_attrs == NULL)
		{
			g_hash_table_new (g_str_hash, g_str_equal);
		}
	g_hash_table_replace (ht_attrs, "zak-cgi-content", g_strdup (content));

	g_hash_table_replace (priv->ht_options, g_strdup (value), ht_attrs);
}

static gchar
*zak_cgi_form_element_select_render (ZakCgiFormElement *element)
{
	gchar *ret;

	GHashTableIter iter;
	gpointer key;
	gpointer value;
	GString *options;
	GHashTable *ht_options_attrs;

	GHashTable *ht_attrs;

	GValue *gval;

	ZakCgiFormElementClass *klass;

	ZakCgiFormElementSelectPrivate *priv = ZAK_CGI_FORM_ELEMENT_SELECT_GET_PRIVATE (element);

	klass = (ZakCgiFormElementClass *)g_type_class_peek_parent (ZAK_CGI_FORM_ELEMENT_SELECT_GET_CLASS (ZAK_CGI_FORM_ELEMENT_SELECT (element)));

	gval = zak_cgi_form_element_get_value (element);

	/* list options */
	options = g_string_new ("\n");
	g_hash_table_iter_init (&iter, priv->ht_options);
	while (g_hash_table_iter_next (&iter, &key, &value))
		{
			ht_options_attrs = (GHashTable *)value;
			g_hash_table_replace (ht_options_attrs, "value", g_strdup ((gchar *)key));

			if (gval != NULL)
				{
					if (g_strcmp0 (g_value_get_string (gval), (gchar *)key) == 0)
						{
							g_hash_table_replace (ht_options_attrs, "selected", g_strdup ("selected"));
						}
				}

			g_string_append (options,
							 zak_cgi_tag_tag_ht ("option", NULL, ht_options_attrs));
			g_string_append (options, "\n");
		}

	ht_attrs = klass->get_ht_attrs (element);

	g_hash_table_replace (ht_attrs, "zak-cgi-content", g_strdup (options->str));
	ret = zak_cgi_tag_tag_ht ("select", zak_cgi_form_element_get_id (element), ht_attrs);

	g_string_free (options, TRUE);

	return ret;
}

static gboolean
zak_cgi_form_element_select_check_value (const gchar *validation_regex, GValue *value)
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
							error != NULL && error->message != NULL ? error->message : "no details");
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
zak_cgi_form_element_select_is_valid (ZakCgiFormElement *element)
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
					if (!zak_cgi_form_element_select_check_value (str_regex, (GValue *)g_ptr_array_index (ar, i)))
						{
							ret = FALSE;
							break;
						}
				}
		}
	else
		{
			ret = zak_cgi_form_element_select_check_value (str_regex, gval);
		}

	return ret;
}

/* PRIVATE */
static void
zak_cgi_form_element_select_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiFormElementSelect *zak_cgi_form_element_select = (ZakCgiFormElementSelect *)object;
	ZakCgiFormElementSelectPrivate *priv = ZAK_CGI_FORM_ELEMENT_SELECT_GET_PRIVATE (zak_cgi_form_element_select);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_element_select_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiFormElementSelect *zak_cgi_form_element_select = (ZakCgiFormElementSelect *)object;
	ZakCgiFormElementSelectPrivate *priv = ZAK_CGI_FORM_ELEMENT_SELECT_GET_PRIVATE (zak_cgi_form_element_select);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_element_select_dispose (GObject *gobject)
{
	ZakCgiFormElementSelect *zak_cgi_form_element_select = (ZakCgiFormElementSelect *)gobject;
	ZakCgiFormElementSelectPrivate *priv = ZAK_CGI_FORM_ELEMENT_SELECT_GET_PRIVATE (zak_cgi_form_element_select);



	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->dispose (gobject);
}

static void
zak_cgi_form_element_select_finalize (GObject *gobject)
{
	ZakCgiFormElementSelect *zak_cgi_form_element_select = (ZakCgiFormElementSelect *)gobject;
	ZakCgiFormElementSelectPrivate *priv = ZAK_CGI_FORM_ELEMENT_SELECT_GET_PRIVATE (zak_cgi_form_element_select);



	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->finalize (gobject);
}
