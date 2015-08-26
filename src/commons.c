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

GHashTable
*zak_cgi_commons_valist_to_ghashtable (va_list ap)
{
	GHashTable *ret;

	gchar *attr;
	gchar *attr_val;

	ret = g_hash_table_new (g_str_hash, g_str_equal);

	do
		{
			attr = va_arg (ap, gchar *);
			if (attr != NULL)
				{
					attr_val = va_arg (ap, gchar *);
					if (attr_val != NULL)
						{
							g_hash_table_insert (ret, g_strdup (attr), g_strdup (attr_val));
						}
					else
						{
							break;
						}
				}
			else
				{
					break;
				}
		} while (TRUE);

	va_end (ap);

	return ret;
}

gchar
*zak_cgi_commons_ghashtable_to_str_attrs (GHashTable *ht)
{
	GHashTableIter iter;
	gpointer key;
	gpointer value;

	GString *str;
	gchar *ret;

	g_return_val_if_fail (ht != NULL, "");

	str = g_string_new ("");

	g_hash_table_iter_init (&iter, ht);
	while (g_hash_table_iter_next (&iter, &key, &value))
		{
			g_string_append_printf (str,
									" \"%s\"=\"%s\"",
									(gchar *)key,
									(gchar *)value);
		}

	ret = g_strdup (str->str);

	return ret;
}
