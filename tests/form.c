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

#include <form.h>
#include <formelement.h>
#include <formelementcheck.h>
#include <formelementhidden.h>
#include <formelementpassword.h>
#include <formelementtext.h>
#include <formelementsubmit.h>

int
main (int argc, char *argv[])
{
	ZakCgiMain *zakcgimain;

	GString *str;

	ZakCgiForm *form;
	ZakCgiFormElement *element;

	zakcgimain = zak_cgi_main_new ();

	str = g_string_new ("<html>\n"
						"<head><title>Form test</title></head>\n"
						"<body>\n");

	form = zak_cgi_form_new (zakcgimain,
							 "method", "post",
							 "action", "form",
							 NULL);

	element = zak_cgi_form_element_text_new ("first", "aaa", NULL);
	zak_cgi_form_element_set_label (element, "The Label for first", NULL);
	zak_cgi_form_add_element (form, element);

	element = zak_cgi_form_element_check_new ("chk", NULL, NULL);
	zak_cgi_form_element_set_label (element, "The checkbox", NULL);
	zak_cgi_form_add_element (form, element);

	zak_cgi_form_add_str (form, "<h1>big big big</h1>");

	element = zak_cgi_form_element_password_new ("pws", "aaa", NULL);
	zak_cgi_form_element_set_label (element, "The password", NULL);
	zak_cgi_form_add_element (form, element);

	element = zak_cgi_form_element_hidden_new ("hdn", "aaa", NULL);
	zak_cgi_form_add_element (form, element);

	element = zak_cgi_form_element_submit_new ("submit", "aaa", NULL);
	zak_cgi_form_add_element (form, element);

	if (zak_cgi_main_is_post (zakcgimain))
		{
			/* validating the form */
			zak_cgi_form_bind (form);
			if (zak_cgi_form_is_valid (form))
				{
					g_string_append (str, "Form is valid!!!");
				}
			else
				{
					g_string_append (str, zak_cgi_form_render (form));
					g_string_append (str, "Form is not valid!!!");
				}
		}
	else
		{
			g_string_append (str, zak_cgi_form_render (form));
		}

	g_string_append (str, "\n</body>");

	zak_cgi_main_out (NULL, str->str);

	return 0;
}
