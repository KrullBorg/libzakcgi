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

#ifndef __ZAK_CGI_URL_H__
#define __ZAK_CGI_URL_H__


#include <glib-object.h>

#include "main.h"


G_BEGIN_DECLS


#define ZAK_CGI_TYPE_URL                 (zak_cgi_url_get_type ())
#define ZAK_CGI_URL(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZAK_CGI_TYPE_URL, ZakCgiUrl))
#define ZAK_CGI_URL_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), ZAK_CGI_TYPE_URL, ZakCgiUrlClass))
#define ZAK_CGI_IS_URL(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZAK_CGI_TYPE_URL))
#define ZAK_CGI_IS_URL_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), ZAK_CGI_TYPE_URL))
#define ZAK_CGI_URL_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), ZAK_CGI_TYPE_URL, ZakCgiUrlClass))

typedef struct _ZakCgiUrl ZakCgiUrl;
typedef struct _ZakCgiUrlClass ZakCgiUrlClass;

struct _ZakCgiUrl
	{
		GObject parent_instance;
	};

struct _ZakCgiUrlClass
	{
		GObjectClass parent_class;
	};

GType zak_cgi_url_get_type (void);


ZakCgiUrl *zak_cgi_url_new (ZakCgiMain *zakcgimain);

typedef void (*ZakCgiUrlConnectedFunction) (GMatchInfo *minfo, GString *buf);

void zak_cgi_url_connect (ZakCgiUrl *url,
						  const gchar *regex,
						  ZakCgiUrlConnectedFunction function,
						  gpointer user_data);

void zak_cgi_url_dispatch (ZakCgiUrl *url);


G_END_DECLS


#endif /* __ZAK_CGI_URL_H__ */
