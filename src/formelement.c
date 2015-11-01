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
#include "formelement.h"
#include "tag.h"

enum
{
	PROP_0
};

static void zak_cgi_form_element_class_init (ZakCgiFormElementClass *class);
static void zak_cgi_form_element_init (ZakCgiFormElement *zak_cgi_form_element);

static void zak_cgi_form_element_construct (ZakCgiFormElement *element,
											const gchar *id,
											GHashTable *ht_attrs);

static GHashTable *zak_cgi_form_element_get_ht_attrs (ZakCgiFormElement *element);

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
		gchar *id;
		GHashTable *ht_attrs;
		GHashTable *ht_label_attrs;
	};

G_DEFINE_TYPE (ZakCgiFormElement, zak_cgi_form_element, ZAK_FORM_TYPE_ELEMENT)

static void
zak_cgi_form_element_class_init (ZakCgiFormElementClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = zak_cgi_form_element_set_property;
	object_class->get_property = zak_cgi_form_element_get_property;
	object_class->dispose = zak_cgi_form_element_dispose;
	object_class->finalize = zak_cgi_form_element_finalize;

	g_type_class_add_private (object_class, sizeof (ZakCgiFormElementPrivate));

	class->construct = zak_cgi_form_element_construct;
	class->get_ht_attrs = zak_cgi_form_element_get_ht_attrs;
}

static void
zak_cgi_form_element_init (ZakCgiFormElement *zak_cgi_form_element)
{
	ZakCgiFormElementPrivate *priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (zak_cgi_form_element);

	priv->ht_attrs = NULL;
	priv->ht_label_attrs = NULL;
}

gchar
*zak_cgi_form_element_get_id (ZakCgiFormElement *element)
{
	ZakCgiFormElementPrivate *priv;

	priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (element);

	return g_strdup (priv->id);
}

/**
 * zak_cgi_form_element_set_label:
 * @element:
 * @label:
 * @...:
 *
 */
void
zak_cgi_form_element_set_label (ZakCgiFormElement *element, const gchar *label, ...)
{
	va_list ap;

	ZakCgiFormElementPrivate *priv;

	priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (element);

	if (priv->ht_label_attrs != NULL)
		{
			g_hash_table_destroy (priv->ht_label_attrs);
		}

	va_start (ap, label);
	priv->ht_label_attrs = zak_cgi_commons_valist_to_ghashtable (ap);

	g_hash_table_replace (priv->ht_label_attrs, "zak-cgi-content", g_strdup (label));
	g_hash_table_replace (priv->ht_label_attrs, "for", g_hash_table_lookup (priv->ht_attrs, "name"));
	g_hash_table_replace (priv->ht_label_attrs, "class", "control-label");
}

/**
 * zak_cgi_form_element_render:
 * @element:
 *
 */
gchar
*zak_cgi_form_element_render (ZakCgiFormElement *element)
{
	GString *str;
	gchar *ret;
	GPtrArray *messages;

	ZakCgiFormElementPrivate *priv;

	priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (element);

	str = g_string_new ("<div class=\"form-group");

	messages = ZAK_FORM_ELEMENT_CLASS (zak_cgi_form_element_parent_class)->get_messages (ZAK_FORM_ELEMENT (element));
	if (messages != NULL)
		{
			g_string_append (str, " has-error");
		}

	g_string_append (str, "\">\n");

	if (priv->ht_label_attrs != NULL)
		{
			gchar *lbl_id;

			lbl_id = g_strdup_printf ("lbl_%s", priv->id);
			g_string_append (str, zak_cgi_tag_tag_ht ("label", lbl_id, priv->ht_label_attrs));
			g_free (lbl_id);
		}

	if (ZAK_CGI_IS_FORM_ELEMENT (element) && ZAK_CGI_FORM_ELEMENT_GET_CLASS (element)->render != NULL)
		{
			gchar *attr_class;

			attr_class = g_hash_table_lookup (priv->ht_attrs, "class");
			if (attr_class != NULL)
				{
					if (g_strstr_len (attr_class, -1, "form-control") == NULL)
						{
							g_hash_table_insert (priv->ht_attrs, "class", g_strdup_printf ("%s form-control", attr_class));
						}
					g_free (attr_class);
				}
			else
				{
					g_hash_table_replace (priv->ht_attrs, g_strdup ("class"), g_strdup ("form-control"));
				}

			g_string_append (str, ZAK_CGI_FORM_ELEMENT_GET_CLASS (element)->render (element));
		}

	if (messages != NULL)
		{
			guint i;

			for (i = 0; i < messages->len; i++)
				{
					g_string_append_printf (str,
											"\n<span id=\"helpBox_%s%d\" class=\"help-block\">%s</span>",
											priv->id,
											i + 1,
											(gchar *)g_ptr_array_index (messages, i));
				}
		}

	g_string_append (str, "\n</div>\n");

	ret = g_strdup (str->str);
	g_string_free (str, TRUE);

	return ret;
}

/* PRIVATE */
static void
zak_cgi_form_element_construct (ZakCgiFormElement *element,
								const gchar *id,
								GHashTable *ht_attrs)
{
	ZakCgiFormElementPrivate *priv;

	g_return_if_fail (id != NULL);

	priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (element);

	priv->id = g_strdup (id);

	priv->ht_attrs = ht_attrs;

	if (g_hash_table_lookup (priv->ht_attrs, "name") == NULL)
		{
			g_hash_table_insert (priv->ht_attrs, "name", g_strdup (id));
		}

	return;
}

static GHashTable
*zak_cgi_form_element_get_ht_attrs (ZakCgiFormElement *element)
{
	ZakCgiFormElementPrivate *priv;

	priv = ZAK_CGI_FORM_ELEMENT_GET_PRIVATE (element);

	return priv->ht_attrs;
}

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
