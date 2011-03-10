/* -*- mode: C -*-
 *
 *       File:         pdf-hash-rename.c
 *       Date:         Wed Mar  12 12:43:00 2008
 *
 *       GNU PDF Library - Unit tests for pdf_hash_rename
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

#include <stdbool.h>
#include <stdio.h>
#include <pdf.h>
#include <check.h>
#include <pdf-test-common.h>
/*
 * Test: pdf_hash_rename_key_001
 * Description:
 *   Try to rename an existent key and prove it was renamed.
 * Success condition:
 *   Returns PDF_TRUE
 */
START_TEST (pdf_hash_rename_key_001)
{
  pdf_hash_t *table;
  pdf_error_t *error = NULL;


  table = pdf_hash_new (NULL);

  pdf_hash_add (table, "key", "val", NULL, NULL);

  fail_if (pdf_hash_rename_key (table, "key", "ke", &error) != PDF_TRUE);
  fail_if (error != NULL);

  fail_if (pdf_hash_key_p (table, "key") == PDF_TRUE);
  fail_if (pdf_hash_key_p (table, "ke") != PDF_TRUE);

  pdf_hash_destroy (table);
}
END_TEST

/*
 * Test: pdf_hash_rename_key_002
 * Description:
 *   Try to rename a non-existent key.
 * Success condition:
 *   Returns PDF_FALSE
 */
START_TEST (pdf_hash_rename_key_002)
{
  pdf_hash_t *table;
  pdf_error_t *error = NULL;


  table = pdf_hash_new (NULL);

  pdf_hash_add (table, "key", "val", NULL, NULL);

  fail_if (pdf_hash_rename_key (table, "ke", "key", &error) == PDF_TRUE);
  fail_if (error != NULL);

  pdf_hash_destroy (table);
}
END_TEST

/*
 * Test case creation function
 */
TCase *
test_pdf_hash_rename_key (void)
{
  TCase *tc = tcase_create ("pdf_hash_rename");

  tcase_add_test (tc, pdf_hash_rename_key_001);
  tcase_add_test (tc, pdf_hash_rename_key_002);
  tcase_add_checked_fixture (tc,
                             pdf_test_setup,
                             pdf_test_teardown);
  return tc;
}

/* End of pdf-hash-rename-key.c */
