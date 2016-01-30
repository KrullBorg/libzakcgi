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

#include <syslog.h>

#include <main.h>
#include <session.h>

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
	gchar *env;
	gchar *username;
	GString *str;
	GString *header;
	GHashTable *ht;
	GHashTable *ht_stdin;
	ZakCgiMain *zakcgimain;
	ZakCgiSession *session;

	gchar *method;

	zakcgimain = zak_cgi_main_new ();

	session = zak_cgi_session_new (zakcgimain, NULL, NULL);

	str = g_string_new ("<html>\n"
	                    "<head><title>Session Cookie</title></head>\n"
	                    "<body>\n");

	g_string_append_printf (str, "<table>\n");
	zak_cgi_main_cookies_foreach (zakcgimain, ht_foreach, str);
	g_string_append_printf (str, "</table>\n");

	ht = zak_cgi_main_get_env (zakcgimain);
	if (ht != NULL)
		{
			method = g_value_get_string (g_hash_table_lookup (ht, "REQUEST_METHOD"));
			if (g_strcmp0 (method, "POST") == 0)
				{
					const gchar *content_type = g_getenv ("CONTENT_TYPE");
					gchar **splitted = g_strsplit (content_type, ";", -1);
					if (g_strv_length (splitted) == 2)
						{
							gchar **boundary = g_strsplit (splitted[1], "=", 2);

							env = zak_cgi_main_get_stdin (zakcgimain);

							ht_stdin = zak_cgi_main_parse_stdin (env, boundary[1]);

							zak_cgi_session_set_value (session, "user_name", (gchar *)g_value_get_string ((GValue *)g_hash_table_lookup (ht_stdin, "user")));

							g_free (env);
							g_strfreev (boundary);
						}
					g_strfreev (splitted);
				}
		}

	username = zak_cgi_session_get_value (session, "user_name");
	if (username == NULL)
		{
			g_string_append (str,
			                 "<form action=\"/cgi-bin/session\" method=\"post\" enctype=\"multipart/form-data\">\n"
			                 "User: <input name=\"user\" /></br>\n"
			                 "Password: <input name=\"password\" type=\"password\" /></br>\n"
			                 "<input type=\"submit\" value=\"Login\" />\n"
			                 "</form>\n");
		}
	else
		{
			g_string_append_printf (str,
			                        "Welcome %s",
			                        username);

			if (g_strcmp0 (method, "POST") == 0)
				{
					g_string_append (str,
					                 "<form action=\"/cgi-bin/session\">\n"
					                 "<input type=\"submit\" value=\"Go forward\" />\n"
					                 "</form>\n");
				}
			else
				{
					g_string_append (str, ", on the second page.");
				}
			g_free (method);
		}

	g_string_append (str, "\n</body>");

	header = g_string_new (ZAK_CGI_STANDARD_HEADER_HTML);
	g_string_append_printf (header, "\n%s", zak_cgi_session_get_header (session));

	zak_cgi_main_out (header->str, str->str);
	g_string_free (str, TRUE);

	if (method != NULL)
		{
			g_free (method);
		}

	return 0;
}
