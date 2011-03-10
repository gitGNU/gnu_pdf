/* -*- mode: C -*-
 *
 *       File:         pdf-text-empty-p.c
 *       Date:         Mon Mar 10 18:30:01 2008
 *
 *       GNU PDF Library - Text Module - pdf_text_empty_p test case
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
 * Test: pdf_text_empty_p_001
 * Description:
 *   Check if a given empty text object is empty
 * Success conditions:
 *   1. The call to pdf_text_empty_p should return PDF_TRUE.
 */
START_TEST (pdf_text_empty_p_001)
{
  pdf_text_t text;




  fail_if(pdf_text_new (&text) != PDF_OK);

  /* 1. The call to pdf_text_empty_p should return PDF_TRUE. */
  fail_unless(pdf_text_empty_p(text) == PDF_TRUE);

  pdf_text_destroy(text);
}
END_TEST


/*
 * Test: pdf_text_empty_p_002
 * Description:
 *   Check if a given non-empty text object is empty
 * Success conditions:
 *   1. The call to pdf_text_empty_p should return PDF_FALSE.
 */
START_TEST (pdf_text_empty_p_002)
{
  pdf_text_t text;




  fail_if(pdf_text_new_from_unicode((pdf_char_t *)"GNU", 3,
                                    PDF_TEXT_UTF8,
                                    &text) != PDF_OK);
  fail_if(text == NULL);

  /* 1. The call to pdf_text_empty_p should return PDF_FALSE. */
  fail_unless(pdf_text_empty_p(text) == PDF_FALSE);

  pdf_text_destroy(text);
}
END_TEST

/*
 * Test case creation function
 */
TCase *
test_pdf_text_empty_p (void)
{
  TCase *tc = tcase_create("pdf_text_empty_p");
  tcase_add_test(tc, pdf_text_empty_p_001);
  tcase_add_test(tc, pdf_text_empty_p_002);
  tcase_add_checked_fixture (tc,
                             pdf_test_setup,
                             pdf_test_teardown);
  return tc;
}

/* End of pdf-text-get-empty-p.c */
