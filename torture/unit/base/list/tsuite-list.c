/* -*- mode: C -*- Time-stamp: "08/03/17 14:32:31 jemarch"
 *
 *       File:         tsuite-list.c
 *       Date:         Wed Mar  12 12:43:00 2008
 *
 *       GNU PDF Library - Testcase definition for the List module
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

#include <check.h>

extern TCase *test_pdf_list_create (void);
extern TCase *test_pdf_list_size (void);
extern TCase *test_pdf_list_add_first (void);
extern TCase *test_pdf_list_add_at (void);
extern TCase *test_pdf_list_add_last (void);
extern TCase *test_pdf_list_set_at (void);
extern TCase *test_pdf_list_get_at (void);
extern TCase *test_pdf_list_remove_at (void);
extern TCase *test_pdf_list_remove_node (void);
extern TCase *test_pdf_list_remove (void);
extern TCase *test_pdf_list_search (void);
extern TCase *test_pdf_list_search_from (void);
extern TCase *test_pdf_list_search_from_to (void);
extern TCase *test_pdf_list_indexof (void);
extern TCase *test_pdf_list_indexof_from (void);
extern TCase *test_pdf_list_indexof_from_to (void);
extern TCase *test_pdf_list_node_value (void);
extern TCase *test_pdf_list_previous_node (void);
extern TCase *test_pdf_list_next_node (void);
extern TCase *test_pdf_list_iterator (void);
extern TCase *test_pdf_list_iterator_from_to (void);
extern TCase *test_pdf_list_iterator_next (void);


Suite *
tsuite_list ()
{
  Suite *s;

  s = suite_create("list");
  
  suite_add_tcase (s, test_pdf_list_create ());
  suite_add_tcase (s, test_pdf_list_size ());
  suite_add_tcase (s, test_pdf_list_add_first ());
  suite_add_tcase (s, test_pdf_list_add_at ());
  suite_add_tcase (s, test_pdf_list_add_last ());
  suite_add_tcase (s, test_pdf_list_node_value ());
  suite_add_tcase (s, test_pdf_list_set_at ());
  suite_add_tcase (s, test_pdf_list_get_at ());
  suite_add_tcase (s, test_pdf_list_remove_at ());
  suite_add_tcase (s, test_pdf_list_remove_node ());
  suite_add_tcase (s, test_pdf_list_remove ());
  suite_add_tcase (s, test_pdf_list_search ());
  suite_add_tcase (s, test_pdf_list_search_from ());
  suite_add_tcase (s, test_pdf_list_search_from_to ());
  suite_add_tcase (s, test_pdf_list_indexof ());
  suite_add_tcase (s, test_pdf_list_indexof_from ());
  suite_add_tcase (s, test_pdf_list_indexof_from_to ());
  suite_add_tcase (s, test_pdf_list_previous_node ());
  suite_add_tcase (s, test_pdf_list_next_node ());
  suite_add_tcase (s, test_pdf_list_iterator ());
  suite_add_tcase (s, test_pdf_list_iterator_from_to ());
  suite_add_tcase (s, test_pdf_list_iterator_next ());

  return s;
}


/* End of tsuite-list.c */