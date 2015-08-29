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

#ifndef __ZAK_CGI_FORM_H__
#define __ZAK_CGI_FORM_H__


#include <glib-object.h>

#include "main.h"
#include "formelement.h"


G_BEGIN_DECLS


#define ZAK_CGI_TYPE_FORM                 (zak_cgi_form_get_type ())
#define ZAK_CGI_FORM(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZAK_CGI_TYPE_FORM, ZakCgiForm))
#define ZAK_CGI_FORM_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), ZAK_CGI_TYPE_FORM, ZakCgiFormClass))
#define ZAK_CGI_IS_FORM(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZAK_CGI_TYPE_FORM))
#define ZAK_CGI_IS_FORM_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), ZAK_CGI_TYPE_FORM))
#define ZAK_CGI_FORM_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), ZAK_CGI_TYPE_FORM, ZakCgiFormClass))

typedef struct _ZakCgiForm ZakCgiForm;
typedef struct _ZakCgiFormClass ZakCgiFormClass;

struct _ZakCgiForm
	{
		GObject parent_instance;
	};

struct _ZakCgiFormClass
	{
		GObjectClass parent_class;
	};

GType zak_cgi_form_get_type (void);


ZakCgiForm *zak_cgi_form_new (ZakCgiMain *zakcgimain, ...);

gboolean zak_cgi_form_add_element (ZakCgiForm *zakcgiform, ZakCgiFormElement *element);
gboolean zak_cgi_form_add_str (ZakCgiForm *zakcgiform, const gchar *str);

void zak_cgi_form_bind (ZakCgiForm *zakcgiform);

gchar *zak_cgi_form_render_start (ZakCgiForm *zakcgiform);
gchar *zak_cgi_form_render (ZakCgiForm *zakcgiform);


G_END_DECLS


#endif /* __ZAK_CGI_FORM_H__ */
