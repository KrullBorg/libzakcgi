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
#include <string.h>

#include <gio/gio.h>

#include <main.h>

gchar
*get_value (GValue *value)
{
	gchar *ret;

	if (G_VALUE_HOLDS (value, ZAK_CGI_TYPE_FILE))
		{
			ZakCgiFile *zgfile = (ZakCgiFile *)g_value_get_boxed ((GValue *)value);

			ret = g_strdup (zgfile->name);

			if (g_strcmp0 (zgfile->name, "") != 0)
				{
					/* save the file to tmp */
					GFile *gfile;
					GFileIOStream *iostream;
					GOutputStream *ostream;

					iostream = NULL;
					gfile = g_file_new_tmp (g_strdup_printf ("cgi-XXXXXX-%s", zgfile->name),
											&iostream,
											NULL);

					ostream = g_io_stream_get_output_stream (G_IO_STREAM (iostream));
					g_output_stream_write (ostream,
										   zgfile->content,
										   zgfile->size,
										   NULL,
										   NULL);
					g_output_stream_close (ostream, NULL, NULL);

					g_object_unref (ostream);
					g_object_unref (gfile);
				}
		}
	else if (G_VALUE_HOLDS (value, G_TYPE_STRING))
		{
			ret = g_strdup ((gchar *)g_value_get_string ((GValue *)value));
		}
	else
		{
			ret = g_strdup ("value of unknown type");
		}

	return ret;
}

void
ht_foreach (gpointer key,
			gpointer value,
			gpointer user_data)
{
	GString *str = (GString *)user_data;

	g_string_append_printf (str, "<tr><td>%s</td><td>%s</td></tr>\n",
							(gchar *)key, g_value_get_string ((GValue *)value));
}

void
ht_foreach_stdin (gpointer key,
			gpointer value,
			gpointer user_data)
{
	gchar *ret;

	GString *str = (GString *)user_data;

	if (G_VALUE_HOLDS (value, G_TYPE_PTR_ARRAY))
		{
			guint i;
			GPtrArray *ar = (GPtrArray *)g_value_get_boxed (value);
			for (i = 0; i < ar->len; i++)
				{
					ret = get_value (g_ptr_array_index (ar, i));
					g_string_append_printf (str,
											"<tr><td>%s[%d]</td><td>%s</td></tr>\n",
											(gchar *)key, i, ret);
					g_free (ret);
				}
		}
	else
		{
			ret = get_value ((GValue *)value);
			g_string_append_printf (str,
									"<tr><td>%s</td><td>%s</td></tr>\n",
									(gchar *)key, ret);
			g_free (ret);
		}
}

int
main (int argc, char *argv[])
{
	ZakCgiMain *zakcgimain;
	GString *str;
	GHashTable *ht;
	gchar *env;
	gchar *ret;

	zakcgimain = zak_cgi_main_new ();

	str = g_string_new ("<html>\n"
						"<head><title>Environment variables</title></head>\n"
						"<body>\n");

	g_string_append_printf (str, "<table>\n");
	zak_cgi_main_env_foreach (zakcgimain, ht_foreach, str);
	g_string_append_printf (str, "</table>\n");

	env = zak_cgi_main_get_stdin (zakcgimain);
	zak_cgi_main_parse_stdin (env, NULL);
	/*syslog (LOG_MAKEPRI(LOG_SYSLOG, LOG_DEBUG), "stdin: %s", env);*/
	if (env != NULL)
		{
			g_string_append_printf (str,
									"<br/><hr/>\n"
									"%s",
									env);

			g_string_append_printf (str, "<br/><hr/>\n<table>\n");

			g_string_append_printf (str,
									"<tr><td>IS GET?</td><td>%s</td></tr>\n",
									zak_cgi_main_is_get (zakcgimain) ? "TRUE" : "FALSE");

			g_string_append_printf (str,
									"<tr><td>IS POST?</td><td>%s</td></tr>\n",
									zak_cgi_main_is_post (zakcgimain) ? "TRUE" : "FALSE");

			zak_cgi_main_stdin_foreach (zakcgimain, ht_foreach_stdin, str);

			g_string_append_printf (str, "</table>\n");
		}

	g_string_append (str, "\n</body>");

	zak_cgi_main_out (NULL, str->str);
	g_string_free (str, TRUE);

	return 0;
}
