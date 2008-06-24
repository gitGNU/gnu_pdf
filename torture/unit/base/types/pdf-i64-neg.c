/* -*- mode: C -*- Time-stamp: ""
 *
 *       File:         pdf-i64-neg.c
 *       Date:         Mon May 05 16:30:00 2008
 *
 *       GNU PDF Library - Types Module - pdf_i64_neg test case
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
 * Test: pdf_i64_neg_001
 * Description:
 *   Checks if the result is the negation of the number
 * Success conditions:
 * The call should produce a negative number from a positive
 */
START_TEST(pdf_i64_neg_001)
{

  int cmp_res;
  pdf_i64_t k,j;

  pdf_i64_assign(&k,0,2); /*2*/
  

#ifndef PDF_USE_BUILTIN_64BIT_SUPPORT 
  fail_if(pdf_i64_neg( &j, k ) != PDF_OK);
  fail_if(j.high != 0xFFFFFFFF);
  fail_if(j.low != 0xFFFFFFFE);
#else
  pdf_i64_neg( &j, k );
  fail_if(j != -2);
#endif
  
  

}
END_TEST


/*
 * Test: pdf_i64_neg_002
 * Description:
 *   Checks if the result is the negation of the number
 * Success conditions:
 * The call should produce a positive number from a negative
 */

START_TEST(pdf_i64_neg_002)
{
  
int cmp_res;
  pdf_i64_t k,j;
  pdf_i64_assign(&k,0xFFFFFFFF,0xFFFFFFFE); /*-2*/

  
  
 
#ifndef PDF_USE_BUILTIN_64BIT_SUPPORT 
  fail_if(pdf_i64_neg( &j, k ) != PDF_OK);  
  fail_if(j.high != 0);
  fail_if(j.low != 2);
#else
  pdf_i64_neg( &j, k );
  fail_if(j != 2);
#endif
  
}
END_TEST








/*
 * Test case creation function
 */
TCase *
test_pdf_i64_neg (void)
{
  TCase *tc = tcase_create("pdf_i64_neg");
  tcase_add_test(tc, pdf_i64_neg_001);
  tcase_add_test(tc, pdf_i64_neg_002);
  return tc;
}

/* End of pdf-i64-neg.c */
