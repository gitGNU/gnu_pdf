/* -*- mode: C -*- Time-stamp: "09/04/17 00:05:59 jemarch"
 *
 *       File:         pdf-time-set-from-u32.c
 *       Date:         Sun Sep 21 16:37:27 2008
 *
 *       GNU PDF Library - Unit tests for pdf_time_set_from_u32
 *
 */

/* Copyright (C) 2009 Free Software Foundation, Inc. */

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
#include <check.h>
#include <pdf.h>
#include <stdlib.h>
/*
 * Test: pdf_time_set_from_u32_001
 * Description:
 *   Create pdf_time_t and pdf_time_span_t objects
 *   and initialize them to with the same number of
 *   seconds. After pdf_time_sub_span pdf_time_t
 *   object schould be equal to 0.
 *
 *Success condition:
 * 1. Function pdf_time_new schould return PDF_OK
 * 2. Function pdf_time_set_from_u32 schould return
 * PDF_OK.
 * 3. Function pdf_time_sub_span schould return
 * PDF_OK.
 * 4. pdf_time_t object is equal to 0.
 *
 */
START_TEST (pdf_time_set_from_u32_001)
{
  pdf_status_t status;
  pdf_time_t time;
  pdf_time_t zero;
  pdf_time_span_t span;
  pdf_u32_t sec;

  status = pdf_time_new(&time);
  fail_if(status != PDF_OK);

  status = pdf_time_new(&zero);
  fail_if(status != PDF_OK);


  span = pdf_time_span_new();

  sec = 0xF2345678;
  status = pdf_time_span_set(&span,0,sec);
  fail_if(status != PDF_OK);

  status = pdf_time_set_from_u32(time, sec);
  fail_if(status != PDF_OK);

  status =  pdf_time_sub_span(time, span);

  fail_unless( pdf_time_cmp(time, zero) == 0 );


  status = pdf_time_destroy(time);
  fail_if(status != PDF_OK);

  status = pdf_time_destroy(zero);
  fail_if(status != PDF_OK);

  status = pdf_time_span_destroy(&span);
  fail_if(status != PDF_OK);
}
END_TEST

/*
 * Test case creation function
 */
TCase *
test_pdf_time_set_from_u32 (void)
{
  TCase *tc = tcase_create ("pdf_time_set_from_u32");

  tcase_add_test(tc, pdf_time_set_from_u32_001);

  return tc;
}

/* End of pdf-time-set-from-u32.c */
