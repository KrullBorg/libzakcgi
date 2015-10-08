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

#ifndef __ZAK_CGI_FORM_ELEMENT_H__
#define __ZAK_CGI_FORM_ELEMENT_H__


#include <glib-object.h>

#include "main.h"
#include "formelementifilter.h"


G_BEGIN_DECLS


#define ZAK_CGI_TYPE_FORM_ELEMENT                 (zak_cgi_form_element_get_type ())
#define ZAK_CGI_FORM_ELEMENT(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZAK_CGI_TYPE_FORM_ELEMENT, ZakCgiFormElement))
#define ZAK_CGI_FORM_ELEMENT_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), ZAK_CGI_TYPE_FORM_ELEMENT, ZakCgiFormElementClass))
#define ZAK_CGI_IS_FORM_ELEMENT(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZAK_CGI_TYPE_FORM_ELEMENT))
#define ZAK_CGI_IS_FORM_ELEMENT_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), ZAK_CGI_TYPE_FORM_ELEMENT))
#define ZAK_CGI_FORM_ELEMENT_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), ZAK_CGI_TYPE_FORM_ELEMENT, ZakCgiFormElementClass))

typedef struct _ZakCgiFormElement ZakCgiFormElement;
typedef struct _ZakCgiFormElementClass ZakCgiFormElementClass;

struct _ZakCgiFormElement
	{
		GObject parent_instance;
	};

struct _ZakCgiFormElementClass
	{
		GObjectClass parent_class;

		void (*construct) (ZakCgiFormElement *element, const gchar *id, const gchar *validation_regex, GHashTable *ht_attrs);
		GHashTable *(*get_ht_attrs) (ZakCgiFormElement *element);

		gchar *(*render) (ZakCgiFormElement *element);
		gboolean (*is_valid) (ZakCgiFormElement *element);
	};


GType zak_cgi_form_element_get_type (void);

gchar *zak_cgi_form_element_get_id (ZakCgiFormElement *element);

void zak_cgi_form_element_set_validation_regex (ZakCgiFormElement *element, const gchar *validation_regex);
gchar *zak_cgi_form_element_get_validation_regex (ZakCgiFormElement *element);

void zak_cgi_form_element_add_filter (ZakCgiFormElement *element, ZakCgiFormElementIFilter *filter);
void zak_cgi_form_element_filter (ZakCgiFormElement *element);

void zak_cgi_form_element_set_value (ZakCgiFormElement *element, GValue *value);
GValue *zak_cgi_form_element_get_value (ZakCgiFormElement *element);

void zak_cgi_form_element_set_label (ZakCgiFormElement *element, const gchar *label, ...);

gchar *zak_cgi_form_element_render (ZakCgiFormElement *element);

gboolean zak_cgi_form_element_is_valid (ZakCgiFormElement *element);


G_END_DECLS


#endif /* __ZAK_CGI_FORM_ELEMENT_H__ */
