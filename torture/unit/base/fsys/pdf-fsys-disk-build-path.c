/* -*- mode: C -*-
 *
 *       File:         pdf-fsys-disk-build-path.c
 *       Date:         Mon Apr  16 00:02:09 2010
 *
 *       GNU PDF Library - Unit tests for pdf_fsys_build_path with the
 *                         Disk filesystem.
 *
 */

/* Copyright (C) 2010-2011 Free Software Foundation, Inc. */

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
 * Test: pdf_fsys_disk_build_path_001
 * Description:
 *   Build a path name from two text objects
 * Success condition:
 *   The call to pdf_fsys_build_path should return PDF_OK
 */
START_TEST (pdf_fsys_disk_build_path_001)
{
  pdf_error_t *error = NULL;
  pdf_text_t *text1, *text2, *text3;
  pdf_text_t *result = NULL;
  const pdf_char_t *first = "want", *second = "some", *third = "beer?";
  pdf_char_t *output_data = NULL;

  text1 = pdf_text_new_from_unicode (first,
                                     strlen (first),
                                     PDF_TEXT_UTF8,
                                     &error);
  fail_unless (text1 != NULL);
  fail_if (error != NULL);

  text2 = pdf_text_new_from_unicode (second,
                                     strlen (second),
                                     PDF_TEXT_UTF8,
                                     &error);
  fail_unless (text2 != NULL);
  fail_if (error != NULL);

  text3 = pdf_text_new_from_unicode (third,
                                     strlen (third),
                                     PDF_TEXT_UTF8,
                                     &error);
  fail_unless (text3 != NULL);
  fail_if (error != NULL);

  result = pdf_fsys_build_path (PDF_FSYS_DISK,
                                &error,
                                text1, text2, text3,
                                NULL);
  fail_unless (result != NULL);
  fail_if (error != NULL);

  output_data = pdf_text_get_unicode (result,
                                      PDF_TEXT_UTF8,
                                      PDF_TEXT_UNICODE_WITH_NUL_SUFFIX,
                                      NULL,
                                      &error);
  fail_unless (output_data != NULL);
  fail_if (error != NULL);

#if FILE_SYSTEM_BACKSLASH_IS_FILE_NAME_SEPARATOR
  fail_unless (strcmp (output_data, "want\\some\\beer?" ) == 0);
  fail_unless (strlen (output_data) == strlen ("want\\some\\beer?"));
#else
  fail_unless (strcmp (output_data, "want/some/beer?" ) == 0);
  fail_unless (strlen (output_data) == strlen ("want/some/beer?"));
#endif /*  FILE_SYSTEM_BACKSLASH_IS_FILE_NAME_SEPARATOR */

  pdf_text_destroy (text1);
  pdf_text_destroy (text2);
  pdf_text_destroy (text3);
  pdf_text_destroy (result);
  pdf_dealloc (output_data);
}
END_TEST

/*
 * Test case creation function
 */
TCase *
test_pdf_fsys_disk_build_path (void)
{
  TCase *tc = tcase_create ("pdf_fsys_disk_build_path");

  tcase_add_test (tc, pdf_fsys_disk_build_path_001);
  tcase_add_checked_fixture (tc,
                             pdf_test_setup,
                             pdf_test_teardown);
  return tc;
}

/* End of pdf-fsys-disk_build-path.c */
