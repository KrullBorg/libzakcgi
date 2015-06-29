/*
 * Copyright (C) 2015 Andrea Zagli <azagli@libero.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <main.h>

int
main (int argc, char *argv[])
{
	GHashTable *ht_env;
	GString *str;

	GHashTableIter iter;
	gpointer key;
	gpointer value;

	ht_env = zak_cgi_main_get_parameters (NULL);

	str = g_string_new ("<html>\n"
	                    "<head><title>Query string</title></head>\n"
	                    "<body>\n");

	if (g_hash_table_size (ht_env) > 0)
		{
			g_string_append_printf (str, "<table>\n");

			g_hash_table_iter_init (&iter, ht_env);
			while (g_hash_table_iter_next (&iter, &key, &value))
				{
					g_string_append_printf (str, "<tr><td>%s</td><td>%s</td></tr>\n",
					                        (gchar *)key, (gchar *)g_value_get_string ((GValue *)value));
				}

			g_string_append_printf (str, "</table>\n");
		}

	g_string_append_printf (str, "</body>\n");

	zak_cgi_main_out (NULL, str->str);
	g_string_free (str, TRUE);

	return 0;
}

