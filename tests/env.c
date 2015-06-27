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

int
main (int argc, char *argv[])
{
	gchar *env;
	GString *str;
	GHashTable *ht;

	env = zak_cgi_main_dump_env ();

	str = g_string_new ("<html>\n"
	                    "<head><title>Environment variables</title></head>\n"
	                    "<body>\n");

	g_string_append_printf (str, "%s\n</body>", env);
	g_free (env);

	env = zak_cgi_main_get_stdin ();
	/*syslog (LOG_MAKEPRI(LOG_SYSLOG, LOG_DEBUG), "stdin: %s", env);*/
	if (env != NULL)
		{
			g_string_append_printf (str,
			                        "<br/><hr/>\n"
			                        "%s",
			                        env);

			const gchar *content_type = g_getenv ("CONTENT_TYPE");
			gchar **splitted = g_strsplit (content_type, ";", -1);
			if (g_strv_length (splitted) == 2)
				{
					gchar **boundary = g_strsplit (splitted[1], "=", 2);

					ht = zak_cgi_main_parse_stdin (env, boundary[1]);

					GHashTableIter iter;

					gpointer key;
					gpointer value;

					if (g_hash_table_size (ht) > 0)
						{
							g_string_append_printf (str, "<br/><hr/>\n<table>\n");

							g_hash_table_iter_init (&iter, ht);
							while (g_hash_table_iter_next (&iter, &key, &value))
								{
									if (G_VALUE_HOLDS (value, G_TYPE_BOXED))
										{
											GPtrArray *ar = (GPtrArray *)g_value_get_boxed ((GValue *)value);

											g_string_append_printf (str, "<tr><td>%s</td><td>%s</td></tr>\n",
											                        (gchar *)key, (gchar *)g_ptr_array_index (ar, 0));

											if (g_strcmp0 ((gchar *)g_ptr_array_index (ar, 0), "") != 0)
												{
													/* save the file to tmp */
													GFile *gfile;
													GFileIOStream *iostream;
													GOutputStream *ostream;

													iostream = NULL;
													gfile = g_file_new_tmp (g_strdup_printf ("cgi-XXXXXX-%s", (gchar *)g_ptr_array_index (ar, 0)),
													                        &iostream,
													                        NULL);

													ostream = g_io_stream_get_output_stream (G_IO_STREAM (iostream));
													g_output_stream_write (ostream,
													                       g_ptr_array_index (ar, 1),
													                       GPOINTER_TO_SIZE (g_ptr_array_index (ar, 2)),
													                       NULL,
													                       NULL);
													g_output_stream_close (ostream, NULL, NULL);
												}
										}
									else
										{
											g_string_append_printf (str, "<tr><td>%s</td><td>%s</td></tr>\n",
											                        (gchar *)key, (gchar *)g_value_get_string ((GValue *)value));
										}
								}

							g_string_append_printf (str, "</table>\n");
						}
				}
			g_strfreev (splitted);

			g_free (env);
		}

	zak_cgi_main_out (NULL, str->str);
	g_string_free (str, TRUE);

	return 0;
}

