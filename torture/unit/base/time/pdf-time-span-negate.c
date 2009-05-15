/* -*- mode: C -*- Time-stamp: "09/05/07 12:27:04 jemarch"
 *
 *       File:         pdf-time-span-set-negate.c
 *       Date:         Sun Sep 21 16:37:27 2008
 *
 *       GNU PDF Library - Unit tests for pdf_time_span_negate
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
 * Test: pdf_time_span_negate_001
 * Description:
 *   Create new pdf_time_span_t object and init it
 *   with positive seconds number.
 *Success condition:
 * 1. Function pdf_time_span_new schould return PDF_OK
 * 2. Function pdf_time_span_negate schould return PDF_OK
 * 3. Seconds returned from pdf_time_span_to_secs
 * schould be expected one (-seconds).
 */
START_TEST (pdf_time_span_negate_001)
{
    pdf_status_t status;
    pdf_time_span_t span;
    pdf_i32_t seconds;
    pdf_i64_t seconds2;

    span=pdf_time_span_new();

    seconds = 0x77234567;
    status = pdf_time_span_set_from_i32(&span,seconds);
    fail_if(status != PDF_OK);

    status = pdf_time_span_negate(&span);
    fail_if(status != PDF_OK);
    seconds = 0 - seconds;

    seconds2 = pdf_time_span_to_secs(span);
    fail_unless(pdf_i64_cmp_i32(seconds2,seconds) == 0);

}
END_TEST

/*
 * Test: pdf_time_span_negate_002
 * Description:
 *   Create new pdf_time_span_t object and init it
 *   with negative seconds number.
 *Success condition:
 * 1. Function pdf_time_span_new schould return PDF_OK
 * 2. Function pdf_time_span_negate schould return PDF_OK
 * 3. Seconds returned from pdf_time_span_to_secs
 * schould be expected one (-seconds).
 */
START_TEST (pdf_time_span_negate_002)
{
    pdf_status_t status;
    pdf_time_span_t span;
    pdf_i32_t seconds;
    pdf_i64_t seconds2;

    span=pdf_time_span_new();

    seconds = -0x01234567;   //negative number
    status = pdf_time_span_set_from_i32(&span,seconds);
    fail_if(status != PDF_OK);

    status = pdf_time_span_negate(&span);
    fail_if(status != PDF_OK);
    seconds = 0 - seconds;

    seconds2 = pdf_time_span_to_secs(span);
    fail_unless(pdf_i64_cmp_i32(seconds2,seconds) == 0);

}
END_TEST

/*
 * Test: pdf_time_span_negate_003
 * Description:
 *   Try to get invoke pdf_time_span_negate where 
 *   argument pointer is NUL.
 *Success condition:
 * 1. Function pdf_time_span_new schould return PDF_OK
 * 2. Function pdf_time_span_negate schould return PDF_ERROR
 */
START_TEST (pdf_time_span_negate_003)
{
    pdf_status_t status;
    pdf_time_span_t span;

    status = pdf_time_span_negate(&span);
    fail_if(status != PDF_ERROR);

}
END_TEST



/*
 * Test case creation function
 */
TCase *
test_pdf_time_span_negate (void)
{
  TCase *tc = tcase_create ("pdf_time_span_negate");

  tcase_add_test(tc, pdf_time_span_negate_001);
  tcase_add_test(tc, pdf_time_span_negate_002);
  tcase_add_test(tc, pdf_time_span_negate_003);


  return tc;
}

/* End of pdf-time-span-negate.c */
