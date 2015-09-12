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

#include <tag.h>

int
main (int argc, char *argv[])
{
	GString *str;
	GString *select_content;

	str = g_string_new ("");

	g_string_append_printf (str, "%s<br/>\n", zak_cgi_tag_tag ("input", "text",
									  "type", "text",
									  "name", "myname",
									  "value", "the content of the text input",
									  NULL));

	g_string_append_printf (str, "%s<br/>\n", zak_cgi_tag_tag ("textarea", "textarea",
									  "zak-cgi-content", "the content of the text area",
									  NULL));

	g_string_append_printf (str, "%s<br/>\n", zak_cgi_tag_tag ("textarea", "textarea-nocont",
									  NULL));

	g_string_append_printf (str, "%s<br/>\n", zak_cgi_tag_img ("theimage", "src", "pippo.png", NULL));

	g_string_append_printf (str, "%s<br/>\n", zak_cgi_tag_text ("thetext", NULL));

	g_string_append_printf (str, "%s<br/>\n", zak_cgi_tag_submit ("submit", NULL));

	select_content = g_string_new ("");
	g_string_append_printf (select_content,
							zak_cgi_tag_tag ("option", NULL,
											 "value", "1",
											 "zak-cgi-content", "One",
											 NULL));
	g_string_append_printf (select_content,
							zak_cgi_tag_tag ("option", NULL,
											 "value", "2",
											 "zak-cgi-content", "Two",
											 NULL));
	g_string_append_printf (select_content,
							zak_cgi_tag_tag ("option", NULL,
											 "value", "3",
											 "zak-cgi-content", "Three",
											 NULL));
	g_string_append_printf (str, "%s<br/>\n", zak_cgi_tag_tag ("select", "idselect", "zak-cgi-content", select_content->str, NULL));

	zak_cgi_main_out (NULL, str->str);

	return 0;
}
