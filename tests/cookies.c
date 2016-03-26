/*
 * Copyright (C) 2015-2016 Andrea Zagli <azagli@libero.it>
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

#include <string.h>

#include <gio/gio.h>

#include <main.h>

void
ht_foreach (gpointer key,
			gpointer value,
			gpointer user_data)
{
	GString *str = (GString *)user_data;

	g_string_append_printf (str, "<tr><td>%s</td><td>%s</td></tr>\n",
							(gchar *)key, g_value_get_string ((GValue *)value));
}

int
main (int argc, char *argv[])
{
	ZakCgiMain *zakcgimain;
	GString *str;
	GString *header;
	GHashTable *ht;

	zakcgimain = zak_cgi_main_new ();

	str = g_string_new ("<html>\n"
	                    "<head><title>Cookies</title></head>\n"
	                    "<body>\n");

	g_string_append_printf (str, "<table>\n");
	zak_cgi_main_cookies_foreach (zakcgimain, ht_foreach, str);
	g_string_append_printf (str, "</table>\n");

	header = g_string_new (ZAK_CGI_STANDARD_HEADER_HTML);
	g_string_append_printf (header,
	                        "\n%s",
	                        zak_cgi_main_set_cookie ("PRIMO", "ilvaloredelcookie1234 56 6 7 7 8",
	                                                 g_date_time_add_months (g_date_time_new_now_utc (), 3), NULL, NULL, FALSE, FALSE));
	g_string_append_printf (header,
	                        "\n%s",
	                        zak_cgi_main_set_cookie ("SECONDO", "il secondo cookie", NULL, NULL, "/", FALSE, TRUE));

	g_message ("header: %s", header->str);

	zak_cgi_main_out (header->str, str->str);
	g_string_free (str, TRUE);

	return 0;
}
