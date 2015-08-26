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
#include "formelementsubmit.h"

static void zak_cgi_form_element_submit_class_init (ZakCgiFormElementSubmitClass *class);
static void zak_cgi_form_element_submit_init (ZakCgiFormElementSubmit *zak_cgi_form_element_submit);

static gchar *zak_cgi_form_element_submit_render (ZakCgiFormElement *element);

static gboolean zak_cgi_form_element_submit_is_valid (ZakCgiFormElement *element);

static void zak_cgi_form_element_submit_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_cgi_form_element_submit_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

static void zak_cgi_form_element_submit_dispose (GObject *gobject);
static void zak_cgi_form_element_submit_finalize (GObject *gobject);

#define ZAK_CGI_FORM_ELEMENT_SUBMIT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ZAK_CGI_TYPE_FORM_ELEMENT_SUBMIT, ZakCgiFormElementSubmitPrivate))

typedef struct _ZakCgiFormElementSubmitPrivate ZakCgiFormElementSubmitPrivate;
struct _ZakCgiFormElementSubmitPrivate
	{
		gpointer empty;
	};

G_DEFINE_TYPE (ZakCgiFormElementSubmit, zak_cgi_form_element_submit, ZAK_CGI_TYPE_FORM_ELEMENT)

static void
zak_cgi_form_element_submit_class_init (ZakCgiFormElementSubmitClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	ZakCgiFormElementClass *elem_class = ZAK_CGI_FORM_ELEMENT_CLASS (klass);

	object_class->set_property = zak_cgi_form_element_submit_set_property;
	object_class->get_property = zak_cgi_form_element_submit_get_property;
	object_class->dispose = zak_cgi_form_element_submit_dispose;
	object_class->finalize = zak_cgi_form_element_submit_finalize;

	elem_class->render = zak_cgi_form_element_submit_render;
	elem_class->is_valid = zak_cgi_form_element_submit_is_valid;

	g_type_class_add_private (object_class, sizeof (ZakCgiFormElementSubmitPrivate));
}

static void
zak_cgi_form_element_submit_init (ZakCgiFormElementSubmit *zak_cgi_form_element_submit)
{
	ZakCgiFormElementSubmitPrivate *priv = ZAK_CGI_FORM_ELEMENT_SUBMIT_GET_PRIVATE (zak_cgi_form_element_submit);

}

/**
 * zak_cgi_form_element_submit_new:
 * @id:
 * @validation_regex:
 * @...:
 *
 * Returns: the newly created #ZakCgiFormElementSubmit object.
 */
ZakCgiFormElement
*zak_cgi_form_element_submit_new (const gchar *id,
								const gchar *validation_regex,
								...)
{
	va_list ap;

	ZakCgiFormElementSubmit *zak_cgi_form_element_submit;

	zak_cgi_form_element_submit = ZAK_CGI_FORM_ELEMENT_SUBMIT (g_object_new (zak_cgi_form_element_submit_get_type (), NULL));

	va_start (ap, validation_regex);

	ZAK_CGI_FORM_ELEMENT_CLASS (zak_cgi_form_element_submit_parent_class)->construct (ZAK_CGI_FORM_ELEMENT (zak_cgi_form_element_submit),
																					id,
																					validation_regex,
																					zak_cgi_commons_valist_to_ghashtable (ap));

	return ZAK_CGI_FORM_ELEMENT (zak_cgi_form_element_submit);
}

static gchar
*zak_cgi_form_element_submit_render (ZakCgiFormElement *element)
{
	gchar *ret;

	GHashTable *ht_attrs;

	ZakCgiFormElementClass *klass;

	klass = (ZakCgiFormElementClass *)g_type_class_peek_parent (ZAK_CGI_FORM_ELEMENT_SUBMIT_GET_CLASS (ZAK_CGI_FORM_ELEMENT_SUBMIT (element)));

	ht_attrs = klass->get_ht_attrs (element);

	ret = zak_cgi_tag_submit_ht (zak_cgi_form_element_get_id (element), ht_attrs);

	return ret;
}

static gboolean
zak_cgi_form_element_submit_is_valid (ZakCgiFormElement *element)
{
	gboolean ret;


	return ret;
}

/* PRIVATE */
static void
zak_cgi_form_element_submit_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiFormElementSubmit *zak_cgi_form_element_submit = (ZakCgiFormElementSubmit *)object;
	ZakCgiFormElementSubmitPrivate *priv = ZAK_CGI_FORM_ELEMENT_SUBMIT_GET_PRIVATE (zak_cgi_form_element_submit);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_element_submit_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakCgiFormElementSubmit *zak_cgi_form_element_submit = (ZakCgiFormElementSubmit *)object;
	ZakCgiFormElementSubmitPrivate *priv = ZAK_CGI_FORM_ELEMENT_SUBMIT_GET_PRIVATE (zak_cgi_form_element_submit);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_cgi_form_element_submit_dispose (GObject *gobject)
{
	ZakCgiFormElementSubmit *zak_cgi_form_element_submit = (ZakCgiFormElementSubmit *)gobject;
	ZakCgiFormElementSubmitPrivate *priv = ZAK_CGI_FORM_ELEMENT_SUBMIT_GET_PRIVATE (zak_cgi_form_element_submit);



	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->dispose (gobject);
}

static void
zak_cgi_form_element_submit_finalize (GObject *gobject)
{
	ZakCgiFormElementSubmit *zak_cgi_form_element_submit = (ZakCgiFormElementSubmit *)gobject;
	ZakCgiFormElementSubmitPrivate *priv = ZAK_CGI_FORM_ELEMENT_SUBMIT_GET_PRIVATE (zak_cgi_form_element_submit);



	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->finalize (gobject);
}
