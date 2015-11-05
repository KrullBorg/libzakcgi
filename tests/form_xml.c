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

#include <unistd.h>

#include <libzakform/libzakform.h>

#include <form.h>
#include <formelement.h>
#include <formelementcheck.h>
#include <formelementhidden.h>
#include <formelementpassword.h>
#include <formelementselect.h>
#include <formelementstring.h>
#include <formelementtext.h>
#include <formelementtextarea.h>
#include <formelementsubmit.h>

int
main (int argc, char *argv[])
{
	ZakCgiMain *zakcgimain;

	GString *str;

	ZakCgiForm *form;
	ZakCgiFormElement *element;

	zakcgimain = zak_cgi_main_new ();

	str = g_string_new ("<!DOCTYPE html>\n"
						"<html lang=\"it\">\n"
						"<head>\n"
						"<meta charset=\"utf-8\">\n"
						"<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n"
						"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
						"<title>Form test</title>\n"
						"<!-- Bootstrap -->\n"
						"<!-- Latest compiled and minified CSS -->\n"
						"<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css\">\n"
						"</head>\n"
						"<body>\n"
						"<div class=\"container\">\n");

	form = zak_cgi_form_new (zakcgimain,
							 "method", "post",
							 "action", "form",
							 NULL);

    GValue *val = zak_cgi_main_get_parameter (zakcgimain, "filename");
	if (zak_form_form_load_from_file (ZAK_FORM_FORM (form), g_value_get_string (val)))
		{
			if (zak_cgi_main_is_post (zakcgimain))
				{
					/* validating the form */
					zak_cgi_form_bind (form);
					if (zak_form_form_is_valid (ZAK_FORM_FORM (form)))
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
		}

	g_string_append (str,
					 "\n</div>\n"
					 "<!-- jQuery (necessary for Bootstrap's JavaScript plugins) -->\n"
					 "<script src=\"http://code.jquery.com/jquery-1.11.3.min.js\"></script>\n"
					 "<!-- Include all compiled plugins (below), or include individual files as needed -->\n"
					 "<!-- Latest compiled and minified JavaScript -->\n"
					 "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/js/bootstrap.min.js\"></script>\n"
					 "</body>\n"
					 "</html>");

	zak_cgi_main_out (NULL, str->str);

	return 0;
}
