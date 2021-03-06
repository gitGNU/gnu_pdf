/* -*- mode: C -*-
 *
 *       File:         pdf-fsys-disk-file-open.c
 *       Date:         Sat May  2 00:02:09 2009
 *
 *       GNU PDF Library - Unit tests for pdf_fsys_file_open with the
 *                         Disk filesystem
 *
 */

/* Copyright (C) 2009-2011 Free Software Foundation, Inc. */

/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>

#include <string.h>
#include <tortutils.h>
#include <check.h>

#include <pdf.h>
#include <pdf-test-common.h>

/*
 * Test: pdf_fsys_disk_file_open_001
 * Description:
 *   Open an existing and readable file for reading
 * Success condition:
 *   The call to pdf_fsys_file_open should return PDF_OK
 */
START_TEST (pdf_fsys_disk_file_open_001)
{
  pdf_error_t *error = NULL;
  pdf_fsys_file_t *file;
  pdf_text_t *path;
  pdf_char_t filename[] = "TD00005", *pathname;

  pathname = tortu_get_data_file_path (filename);

  path = pdf_text_new_from_unicode (pathname,
                                    strlen (pathname),
                                    PDF_TEXT_UTF8,
                                    &error);
  fail_unless (path != NULL);
  fail_if (error != NULL);

  file = pdf_fsys_file_open (PDF_FSYS_DISK,
                             path,
                             PDF_FSYS_OPEN_MODE_READ,
                             &error);
  fail_unless (file != NULL);
  fail_if (error != NULL);

  fail_unless (pdf_fsys_file_close (file, &error) == PDF_TRUE);
  fail_if (error != NULL);

  pdf_text_destroy (path);
}
END_TEST

/*
 * Test: pdf_fsys_disk_file_open_002
 * Description:
 *   Open a non-existing file for reading
 * Success condition:
 *   The call to pdf_fsys_file_open should return PDF_EBADNAME
 */
START_TEST (pdf_fsys_disk_file_open_002)
{
  pdf_error_t *error = NULL;
  pdf_fsys_file_t *file;
  pdf_text_t *path;
  pdf_char_t filename[] = "DOESNTEXIST";

  path = pdf_text_new_from_unicode (filename,
                                    strlen (filename),
                                    PDF_TEXT_UTF8,
                                    &error);
  fail_unless (path != NULL);
  fail_if (error != NULL);

  file = pdf_fsys_file_open (PDF_FSYS_DISK,
                             path,
                             PDF_FSYS_OPEN_MODE_READ,
                             &error);
  fail_if (file != NULL);
  fail_unless (error != NULL);
  fail_unless (pdf_error_get_status (error) == PDF_EBADNAME);

  pdf_text_destroy (path);
}
END_TEST

/*
 * Test: pdf_fsys_disk_file_open_003
 * Description:
 *   Open an existing but readonly file for writing
 * Success condition:
 *   The call to pdf_fsys_file_open should return PDF_EBADPERMS
 */
START_TEST (pdf_fsys_disk_file_open_003)
{
  pdf_error_t *error = NULL;
  pdf_fsys_file_t *file;
  pdf_text_t *path;
  pdf_char_t filename[] = "TD00005", *pathname;

  pathname = tortu_get_data_file_path (filename);

  path = pdf_text_new_from_unicode (pathname,
                                    strlen (pathname),
                                    PDF_TEXT_UTF8,
                                    &error);
  fail_unless (path != NULL);
  fail_if (error != NULL);


  file = pdf_fsys_file_open (PDF_FSYS_DISK,
                             path,
                             PDF_FSYS_OPEN_MODE_WRITE,
                             &error);
  fail_if (file != NULL);
  fail_unless (error != NULL);
  fail_unless (pdf_error_get_status (error) == PDF_EBADPERMS);

  pdf_text_destroy (path);
}
END_TEST

/*
 * Test: pdf_fsys_disk_file_open_004
 * Description:
 *   Open an existing non readable but writeable file for writing
 * Success condition:
 *   The call to pdf_fsys_file_open should return PDF_OK
 */
START_TEST (pdf_fsys_disk_file_open_004)
{
  pdf_error_t *error = NULL;
  pdf_fsys_file_t *file;
  pdf_text_t *path;
  pdf_char_t filename[] = "TD00004", *pathname;

  pathname = tortu_get_data_file_path (filename);

  path = pdf_text_new_from_unicode (pathname,
                                    strlen (pathname),
                                    PDF_TEXT_UTF8,
                                    &error);
  fail_unless (path != NULL);
  fail_if (error != NULL);


  file = pdf_fsys_file_open (PDF_FSYS_DISK,
                             path,
                             PDF_FSYS_OPEN_MODE_WRITE,
                             &error);
  fail_unless (file != NULL);
  fail_if (error != NULL);

  fail_unless (pdf_fsys_file_close (file, &error) == PDF_TRUE);
  fail_if (error != NULL);

  pdf_text_destroy (path);
}
END_TEST

/*
 * Test case creation function
 */
TCase *
test_pdf_fsys_disk_file_open (void)
{
  TCase *tc = tcase_create ("pdf_fsys_disk_file_open");

  tcase_add_test (tc, pdf_fsys_disk_file_open_001);
  tcase_add_test (tc, pdf_fsys_disk_file_open_002);
  tcase_add_test (tc, pdf_fsys_disk_file_open_003);
  tcase_add_test (tc, pdf_fsys_disk_file_open_004);
  tcase_add_checked_fixture (tc,
                             pdf_test_setup,
                             pdf_test_teardown);
  return tc;
}

/* End of pdf-fsys-disk-file-open.c */
