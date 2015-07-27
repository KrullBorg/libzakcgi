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

#include "tag.h"

static GPtrArray
*zak_cgi_tag_valist_to_gptrarray (va_list ap)
{
	GPtrArray *ret;

	gchar *attr;

	ret = g_ptr_array_new ();

	do
		{
			attr = va_arg (ap, gchar *);
			if (attr != NULL)
				{
					g_ptr_array_add (ret, g_strdup (attr));
				}
			else
				{
					break;
				}
		} while (TRUE);
	va_end (ap);

	return ret;
}

static gchar
*zak_cgi_tag_tag_attrs (const gchar *name,
						const gchar *id,
						GPtrArray *ar_attrs)
{
	gchar *ret;
	GString *str;

	guint i;
	guint l;

	gboolean with_name;
	gboolean with_content;

	gchar *attr;
	gchar *attr_val;

	gchar *content;

	with_name = FALSE;
	with_content = FALSE;

	content = NULL;

	str = g_string_new ("");
	g_string_append_printf (str,
							"<%s id=\"%s\"",
							name,
							id);

	l = ar_attrs->len;
	for (i = 0; i < l; i++)
		{
			attr = g_ptr_array_index (ar_attrs, i);
			if (attr != NULL)
				{
					attr_val = g_ptr_array_index (ar_attrs, ++i);
					if (attr_val != NULL)
						{
							if (g_strcmp0 (attr, "content") == 0)
								{
									with_content = TRUE;
									if (content != NULL)
										{
											g_free (content);
										}
									content = g_strdup (attr_val);
								}
							else
								{
									if (g_strcmp0 (attr, "name") == 0)
										{
											with_name = TRUE;
										}

									g_string_append_printf (str,
															" %s=\"%s\"",
															attr,
															attr_val);
								}
						}
					else
						{
							g_free (attr);
							break;
						}
				}
			else
				{
					break;
				}
		}

	if (!with_name)
		{
			g_string_append_printf (str,
									" name=\"%s\"",
									id);
		}

	if (!with_content)
		{
			g_string_append (str,
							 " />");
		}
	else
		{
			g_string_append_printf (str,
									">%s</%s>",
									content,
									name);
		}

	ret = g_strdup (str->str);
	g_string_free (str, TRUE);

	return ret;
}

/**
 * zak_cgi_tag_tag:
 * @name:
 * @id:
 *
 * Returns:
 */
gchar
*zak_cgi_tag_tag (const gchar *name,
				  const gchar *id,
				  ...)
{
	va_list ap;

	va_start (ap, id);

	return zak_cgi_tag_tag_attrs (name, id, zak_cgi_tag_valist_to_gptrarray (ap));
}

/**
 * zak_cgi_tag_img:
 * @name:
 * @id:
 *
 * Returns:
 */
gchar
*zak_cgi_tag_img (const gchar *id,
				  ...)
{
	va_list ap;

	va_start (ap, id);

	return zak_cgi_tag_tag_attrs ("img", id, zak_cgi_tag_valist_to_gptrarray (ap));
}

/**
 * zak_cgi_tag_text:
 * @name:
 * @id:
 *
 * Returns:
 */
gchar
*zak_cgi_tag_text (const gchar *id,
				   ...)
{
	GPtrArray *ar;
	va_list ap;

	va_start (ap, id);

	ar = zak_cgi_tag_valist_to_gptrarray (ap);
	g_ptr_array_add (ar, "type");
	g_ptr_array_add (ar, "text");

	return zak_cgi_tag_tag_attrs ("input", id, ar);
}

/**
 * zak_cgi_tag_file:
 * @name:
 * @id:
 *
 * Returns:
 */
gchar
*zak_cgi_tag_file (const gchar *id,
				   ...)
{
	GPtrArray *ar;
	va_list ap;

	va_start (ap, id);

	ar = zak_cgi_tag_valist_to_gptrarray (ap);
	g_ptr_array_add (ar, "type");
	g_ptr_array_add (ar, "file");

	return zak_cgi_tag_tag_attrs ("input", id, ar);
}
