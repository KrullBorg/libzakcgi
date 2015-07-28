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

#include <url.h>

void
hook (GMatchInfo *minfo, gpointer user_data)
{
	GString *str = (GString *)user_data;

	g_string_append_printf (str, "FROM THE HOOK<br/><br/>\n");

	while (g_match_info_matches (minfo))
		{
			guint n = g_match_info_get_match_count (minfo);
			g_string_append_printf (str, "Match count: %d<br/><br/>\n", n);
			gchar *word = g_match_info_fetch (minfo, 0);
			g_string_append_printf (str, "Found: %s<br/><br/>\n", word);
			g_free (word);

			guint i;
			for (i = 1; i < n; i++)
				{
					gchar *word = g_match_info_fetch (minfo, i);
					g_string_append_printf (str, "sub %d: %s<br/><br/>\n", i, word);
					g_free (word);
				}

			if (n > 1)
				{
					word = g_match_info_fetch_named (minfo, "controller");
					g_string_append_printf (str, "sub named controller: %s<br/><br/>\n", word);
					g_free (word);
				}

			g_match_info_next (minfo, NULL);
		}
}

int
main (int argc, char *argv[])
{
	gchar *env;
	ZakCgiUrl *url;
	GString *str;

	str = g_string_new ("<html>\n"
	                    "<head><title>Url</title></head>\n"
	                    "<body>\n"
						"FROM INIT<br/><br/>\n");

	env = zak_cgi_main_dump_env (NULL);

	url = zak_cgi_url_new (NULL);

	zak_cgi_url_connect (url, "/(?<controller>[a-zA-Z0-9_-]+)/([a-zA-Z0-9_-]+)", (ZakCgiUrlConnectedFunction)hook, str);

	zak_cgi_url_dispatch (url);

	g_string_append_printf (str, "<hr/>%s</body>\n", env);

	zak_cgi_main_out (NULL, str->str);
	g_string_free (str, TRUE);

	return 0;
}
