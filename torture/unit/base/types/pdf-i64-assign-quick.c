/* -*- mode: C -*- Time-stamp: ""
 *
 *       File:         pdf-i64-assign-quick.c
 *       Date:         Mon May 05 10:30:01 2008
 *
 *       GNU PDF Library - Types Module - pdf_i64_assign_quick test case
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
#include <string.h>
#include <pdf.h>
#include <check.h>



#define INTERACTIVE_DEBUG 0

/*
 * Test: pdf_i64_assign_001
 * Description:
 *   Checks if the the pdf_i64_assign function properly assigns
 *  the high and low values to a pdf_i64_t type
 * Success conditions:
 * The call should not produce an error
 */
START_TEST(pdf_i64_assign_quick_001)
{

  pdf_i64_t k;

  fail_if(pdf_i64_assign_quick(&k,5) != PDF_OK);
  
  fail_if(k.low != 5);
  fail_if(k.high != 0);
  

}
END_TEST

/*
 * Test: pdf_i64_assign_002
 * Description:
 *   Checks if the the pdf_i64_assign function properly assigns
 *  the high and low values to a pdf_i64_t type
 * Success conditions:
 * The call should produce an error
 */
START_TEST(pdf_i64_assign_quick_002)
{

  pdf_i64_t* k = NULL;

  fail_if(pdf_i64_assign_quick(k,5) != PDF_ERROR);
  
  
}
END_TEST



/*
 * Test case creation function
 */
TCase *
test_pdf_i64_assign_quick (void)
{
  TCase *tc = tcase_create("pdf_i64_assign_quick");
  tcase_add_test(tc, pdf_i64_assign_quick_001);
  tcase_add_test(tc, pdf_i64_assign_quick_002);
  return tc;
}

/* End of pdf-i64-assign-quick.c */
