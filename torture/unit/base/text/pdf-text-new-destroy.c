/* -*- mode: C -*-
 *
 *       File:         pdf-text-new-destroy.c
 *       Date:         Mon Mar 10 18:30:01 2008
 *
 *       GNU PDF Library - Text Module - pdf_text_new_destroy test case
 *
 */

/* Copyright (C) 2008-2011 Free Software Foundation, Inc. */

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
#include <stdio.h>
#include <string.h>
#include <pdf.h>
#include <check.h>
#include <pdf-test-common.h>

/*
 * Test: pdf_text_new_destroy_001
 * Description:
 *   Create an empty text object and destroy it
 * Success conditions:
 *   1. The call to @code{pdf_text_new} should not return a @code{NULL} pointer.
 *   2. The text data of the created object should be empty.
 *   3. The language code of the output object should be empty
 *   4. The country code of the output object should be empty
 *   5. The call to @code{pdf_text_destroy} should succeed.
 */
START_TEST (pdf_text_new_destroy_001)
{
  pdf_error_t *error = NULL;
  pdf_text_t *newtext;

  /* 1. The call to @code{pdf_text_new} should not return an error
     code. */
  newtext = pdf_text_new (&error);
  fail_unless (newtext != NULL);
  fail_if (error != NULL);

  /* 2. The text data of the created object should be empty. */
  fail_unless (pdf_text_empty_p (newtext) == PDF_TRUE);

  /* 3. The language code of the output object should be empty */
  fail_unless (strlen (pdf_text_get_language (newtext)) == 0);

  /* 4. The country code of the output object should be empty */
  fail_unless (strlen (pdf_text_get_country (newtext)) == 0);

  /* 5. The call to @code{pdf_text_destroy} should succeed. Again,
   *  probably this test is not that useful... */
  pdf_text_destroy (newtext);
}
END_TEST

/*
 * Test case creation function
 */
TCase *
test_pdf_text_new_destroy (void)
{
  TCase *tc = tcase_create ("pdf_text_new_destroy");

  tcase_add_test (tc, pdf_text_new_destroy_001);

  tcase_add_checked_fixture (tc,
                             pdf_test_setup,
                             pdf_test_teardown);
  return tc;
}

/* End of pdf-text-new-destroy.c */
