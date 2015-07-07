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
hook (gpointer user_data)
{
	GString *str = (GString *)user_data;

	g_string_append_printf (str, "FROM THE HOOK<br/><br/>\n");
}

int
main (int argc, char *argv[])
{
	ZakCgiUrl *url;
	GString *str;

	str = g_string_new ("<html>\n"
	                    "<head><title>Url</title></head>\n"
	                    "<body>\n");

	url = zak_cgi_url_new (NULL);

	zak_cgi_url_connect (url, "thecontroller", "theaction", (ZakCgiUrlConnectedFunction)hook, str);

	zak_cgi_url_dispatch (url);

	g_string_append_printf (str, "</body>\n");

	zak_cgi_main_out (NULL, str->str);
	g_string_free (str, TRUE);

	return 0;
}

