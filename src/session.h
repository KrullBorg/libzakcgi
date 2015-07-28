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

#ifndef __ZAK_CGI_SESSION_H__
#define __ZAK_CGI_SESSION_H__


#include <glib-object.h>

#include "main.h"


G_BEGIN_DECLS


#define ZAK_CGI_TYPE_SESSION                 (zak_cgi_session_get_type ())
#define ZAK_CGI_SESSION(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZAK_CGI_TYPE_SESSION, ZakCgiSession))
#define ZAK_CGI_SESSION_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), ZAK_CGI_TYPE_SESSION, ZakCgiSessionClass))
#define ZAK_CGI_IS_SESSION(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZAK_CGI_TYPE_SESSION))
#define ZAK_CGI_IS_SESSION_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), ZAK_CGI_TYPE_SESSION))
#define ZAK_CGI_SESSION_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), ZAK_CGI_TYPE_SESSION, ZakCgiSessionClass))

typedef struct _ZakCgiSession ZakCgiSession;
typedef struct _ZakCgiSessionClass ZakCgiSessionClass;

struct _ZakCgiSession
	{
		GObject parent_instance;
	};

struct _ZakCgiSessionClass
	{
		GObjectClass parent_class;
	};

GType zak_cgi_session_get_type (void);


ZakCgiSession *zak_cgi_session_new (ZakCgiMain *zakcgimain,
									const gchar *base_uri,
									const gchar *path);

gchar *zak_cgi_session_get_header (ZakCgiSession *session);

void zak_cgi_session_set_value (ZakCgiSession *session, const gchar *name, const gchar *value);
gchar *zak_cgi_session_get_value (ZakCgiSession *session, const gchar *name);


G_END_DECLS


#endif /* __ZAK_CGI_SESSION_H__ */
