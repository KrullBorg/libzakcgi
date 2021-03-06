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

void
ht_foreach (gpointer key,
			gpointer value,
			gpointer user_data)
{
	GString *str = (GString *)user_data;

	if (G_VALUE_HOLDS ((GValue *)value, G_TYPE_BOXED))
		{
			guint i;
			GPtrArray *ar = (GPtrArray *)g_value_get_boxed ((GValue *)value);
			for (i = 0; i < ar->len; i++)
				{
					g_string_append_printf (str,
											"<tr><td>%s[%d]</td><td>%s</td></tr>\n",
											(gchar *)key,
											i,
											(gchar *)g_ptr_array_index (ar, i));
				}
		}
	else
		{
			g_string_append_printf (str,
									"<tr><td>%s</td><td>%s</td></tr>\n",
									(gchar *)key,
									(gchar *)g_value_get_string ((GValue *)value));
		}
}

int
main (int argc, char *argv[])
{
	GString *str;

	ZakCgiMain *main;

	main = zak_cgi_main_new ();

	str = g_string_new ("<html>\n"
	                    "<head><title>Query string</title></head>\n"
	                    "<body>\n");

	g_string_append_printf (str, "<table>\n");

	g_string_append_printf (str,
							"<tr><td>IS GET?</td><td>%s</td></tr>\n",
							zak_cgi_main_is_get (main) ? "TRUE" : "FALSE");

	g_string_append_printf (str,
							"<tr><td>IS POST?</td><td>%s</td></tr>\n",
							zak_cgi_main_is_post (main) ? "TRUE" : "FALSE");

	zak_cgi_main_parameters_foreach (main, ht_foreach, str);

	g_string_append_printf (str, "</table>\n");

	g_string_append_printf (str, "</body>\n");

	zak_cgi_main_out (NULL, str->str);
	g_string_free (str, TRUE);

	return 0;
}
