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

#ifndef __ZAK_CGI_FORM_ELEMENT_FILTER_TRIM_H__
#define __ZAK_CGI_FORM_ELEMENT_FILTER_TRIM_H__


#include <glib-object.h>


G_BEGIN_DECLS


/*
 * Type declaration.
 */
#define ZAK_CGI_TYPE_FORM_ELEMENT_FILTER_TRIM zak_cgi_form_element_filter_trim_get_type ()
G_DECLARE_FINAL_TYPE (ZakCgiFormElementFilterTrim, zak_cgi_form_element_filter_trim, ZAK_CGI, FORM_ELEMENT_FILTER_TRIM, GObject)

/*
 * Method definitions.
 */
ZakCgiFormElementFilterTrim *zak_cgi_form_element_filter_trim_new (void);


G_END_DECLS


#endif /* __ZAK_CGI_FORM_ELEMENT_FILTER_TRIM_H__ */