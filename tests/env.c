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
	gchar *env;
	GString *str;

	env = zak_cgi_main_dump_env ();

	str = g_string_new ("<html>\n"
	                    "<head><title>Environment variables</title></head>\n"
	                    "<body>\n");

	g_string_append_printf (str, "%s\n</body>", env);
	g_free (env);

	env = zak_cgi_main_get_stdin ();
	if (env != NULL)
		{
			g_string_append_printf (str,
			                        "<br/><hr/>\n"
			                        "%s",
			                        env);
			g_free (env);
		}

	zak_cgi_main_out (str->str);
	g_string_free (str, TRUE);

	return 0;
}

