/* -*- mode: C -*-
 *
 *       File:         pdf-hash-add-stm.c
 *       Date:         Wed Mar  12 12:43:00 2008
 *
 *       GNU PDF Library - Unit tests for pdf_hash_add_stm
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
 * Test: pdf_hash_add_stm_001
 * Description:
 *   Try to add a stream inside a hash table.
 * Success condition:
 *   Returns PDF_TRUE
 */
START_TEST (pdf_hash_add_stm_001)
{
  pdf_hash_t *table;
  pdf_stm_t *inner;
  char mem[4];
  pdf_error_t *error = NULL;


  table = pdf_hash_new (NULL);

  inner = pdf_stm_mem_new (mem, 4, 0, PDF_STM_READ, &error);
  fail_unless (inner != NULL);
  fail_if (error != NULL);

  fail_if (pdf_hash_add_stm (table,
                             "theKey",
                             inner,
                             &error) != PDF_TRUE);
  fail_if (error != NULL);

  pdf_hash_destroy (table);
}
END_TEST

/*
 * Test case creation function
 */
TCase *
test_pdf_hash_add_stm (void)
{
  TCase *tc = tcase_create ("pdf_hash_add_stm");

  tcase_add_test (tc, pdf_hash_add_stm_001);
  tcase_add_checked_fixture (tc,
                             pdf_test_setup,
                             pdf_test_teardown);
  return tc;
}

/* End of pdf-hash-add-stm.c */
