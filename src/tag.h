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

#ifndef __ZAK_CGI_TAG_H__
#define __ZAK_CGI_TAG_H__


#include <glib-object.h>

#include "main.h"


gchar *zak_cgi_tag_tag (const gchar *name,
						const gchar *id,
						...);

gchar *zak_cgi_tag_tag_ht (const gchar *name,
						   const gchar *id,
						   GHashTable *ht);

gchar *zak_cgi_tag_img (const gchar *id,
						...);
gchar *zak_cgi_tag_img_ht (const gchar *id,
						   GHashTable *ht);

gchar *zak_cgi_tag_text (const gchar *id,
						 ...);
gchar *zak_cgi_tag_text_ht (const gchar *id,
							GHashTable *ht);

gchar *zak_cgi_tag_file (const gchar *id,
						 ...);
gchar *zak_cgi_tag_file_ht (const gchar *id,
							GHashTable *ht);

gchar *zak_cgi_tag_submit (const gchar *id,
						   ...);
gchar *zak_cgi_tag_submit_ht (const gchar *id,
							  GHashTable *ht);


#endif /* __ZAK_CGI_TAG_H__ */
