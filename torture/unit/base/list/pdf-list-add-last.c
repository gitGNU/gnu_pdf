/* -*- mode: C -*-
 *
 *       File:         pdf-list-add-last.c
 *       Date:         Wed Mar  12 12:43:00 2008
 *
 *       GNU PDF Library - Unit tests for pdf_list_add_last
 *
 */

/* Copyright (C) 2008 Free Software Foundation, Inc. */

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


/*
 * Test: pdf_list_add_last_001
 * Description:
 *   Try to add some elements.
 * Success condition:
 *   We get the right pdf_list_size().
 */
START_TEST (pdf_list_add_last_001)
{
  pdf_list_t list;
  int elem, elem2;

  elem = 12345;
  elem2 = 4567;

  pdf_init();

  pdf_list_new (NULL, NULL, 0, &list);

  pdf_list_add_last (list, &elem, NULL);
  fail_if (pdf_list_size(list) != 1);

  pdf_list_add_last (list, &elem2, NULL);
  fail_if (pdf_list_size(list) != 2);

  pdf_list_destroy (list);
}
END_TEST

/*
 * Test case creation function
 */
TCase *
test_pdf_list_add_last (void)
{
  TCase *tc = tcase_create("pdf_list_add_last");
  tcase_add_test(tc, pdf_list_add_last_001);

  return tc;
}

/* End of pdf-list-add-last.c */
