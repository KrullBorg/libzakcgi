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

#include "commons.h"
#include "tag.h"

static gchar
*zak_cgi_tag_tag_attrs (const gchar *name,
						const gchar *id,
						GHashTable *ht_attrs)
{
	gchar *ret;
	GString *str;

	GHashTableIter iter;

	gboolean with_name;
	gboolean with_content;

	gpointer key;
	gpointer val;

	gchar *content;

	with_name = FALSE;
	with_content = FALSE;

	content = NULL;

	str = g_string_new ("");
	g_string_append_printf (str,
							"<%s",
							name);

	if (id != NULL)
		{
			g_string_append_printf (str,
									" id=\"%s\"",
									id);
		}

	g_hash_table_iter_init (&iter, ht_attrs);
	while (g_hash_table_iter_next (&iter, &key, &val))
		{
			if (g_strcmp0 ((gchar *)key, "content") == 0)
				{
					with_content = TRUE;
					if (content != NULL)
						{
							g_free (content);
						}
					content = g_strdup ((gchar *)val);
				}
			else
				{
					if (g_strcmp0 ((gchar *)key, "name") == 0)
						{
							with_name = TRUE;
						}

					g_string_append_printf (str,
											" %s=\"%s\"",
											(gchar *)key,
											(gchar *)val);
				}
		}

	if (!with_name
		&& id != NULL)
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

	return zak_cgi_tag_tag_attrs (name, id, zak_cgi_commons_valist_to_ghashtable (ap));
}

/**
 * zak_cgi_tag_img:
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

	return zak_cgi_tag_tag_attrs ("img", id, zak_cgi_commons_valist_to_ghashtable (ap));
}

/**
 * zak_cgi_tag_text:
 * @id:
 *
 * Returns:
 */
gchar
*zak_cgi_tag_text (const gchar *id,
				   ...)
{
	GHashTable *ht;
	va_list ap;

	va_start (ap, id);

	ht = zak_cgi_commons_valist_to_ghashtable (ap);
	g_hash_table_insert (ht, "type", "text");

	return zak_cgi_tag_tag_attrs ("input", id, ht);
}

/**
 * zak_cgi_tag_file:
 * @id:
 *
 * Returns:
 */
gchar
*zak_cgi_tag_file (const gchar *id,
				   ...)
{
	GHashTable *ht;
	va_list ap;

	va_start (ap, id);

	ht = zak_cgi_commons_valist_to_ghashtable (ap);
	g_hash_table_insert (ht, "type", "file");

	return zak_cgi_tag_tag_attrs ("input", id, ht);
}

/**
 * zak_cgi_tag_submit:
 * @id:
 *
 * Returns:
 */
gchar
*zak_cgi_tag_submit (const gchar *id,
					 ...)
{
	GHashTable *ht;
	va_list ap;

	va_start (ap, id);

	ht = zak_cgi_commons_valist_to_ghashtable (ap);
	g_hash_table_insert (ht, "type", "submit");

	return zak_cgi_tag_tag_attrs ("input", id, ht);
}
