/* -*- mode: C -*-
 *
 *       File:         pdf-list-sorted-add.c
 *       Date:         Wed Mar  12 12:43:00 2008
 *
 *       GNU PDF Library - Unit tests for pdf_list_sorted_add
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
#include <pdf.h>
#include <check.h>

#include "pdf-list-test-common.h"

/*
 * Test: pdf_list_sorted_add_001
 * Description:
 *   Try to add an element at the beginning of the list.
 * Success condition:
 *   Returns the newly added node.
 */
START_TEST (pdf_list_sorted_add_001)
{
  pdf_list_t *list;
  int elem;
  pdf_error_t *error = NULL;
  pdf_list_node_t *node;

  elem = 5123;

  pdf_init ();

  list = pdf_list_new (l_comp, NULL, PDF_FALSE, NULL);

  node = pdf_list_sorted_add (list, l_comp_asc, &elem, &error);
  fail_if (node == NULL);
  fail_if (error != NULL);

  pdf_list_destroy (list);
}
END_TEST

/*
 * Test case creation function
 */
TCase *
test_pdf_list_sorted_add (void)
{
  TCase *tc = tcase_create ("pdf_list_sorted_add");

  tcase_add_test (tc, pdf_list_sorted_add_001);
  return tc;
}

/* End of pdf-list-sorted-add.c */
