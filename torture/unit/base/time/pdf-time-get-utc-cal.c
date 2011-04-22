/* -*- mode: C -*-
 *
 *       File:         pdf-time-get-utc-cal.c
 *       Date:         Fri Feb 27 17:35:31 2008
 *
 *       GNU PDF Library - Unit tests for pdf_time_new
 *
 */

/* Copyright (C) 2009-2011 Free Software Foundation, Inc. */

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
#include <time.h>
#include <base/time/pdf-time-test-common.h>
#include <pdf-test-common.h>

/*
 * Test: pdf_time_get_utc_cal_001
 * Description:
 *   Create new pdf_time_t and initialize it with a
 *   date in utc. Returned localcal schould be
 *   expected one.
 * Success condition:
 *   1. Function pdf_time_new schould return PDF_OK
 *   2. Function pdf_time_set_from_u32 schould return
 *      PDF_OK.
 *   3. Function pdf_time_get_local_cal schould return
 *      PDF_OK.
 *   4. Returned localcal must be expected one.
 */
START_TEST (pdf_time_get_utc_cal_001)
{
  extern pdf_u32_t dates_in_seconds[];
  extern struct pdf_time_cal_s dates[];
  pdf_time_t time1;
  pdf_u32_t i;

  pdf_time_init (&time1);

  for (i = 0; i < DATES_SIZE; i++)
    {
      struct pdf_time_cal_s utccal;

      pdf_time_set_utc (&time1, dates_in_seconds[i]);
      pdf_time_get_utc_cal (&time1, &utccal);
      fail_unless (memcmp (&utccal,
                           &dates[i],
                           sizeof (struct pdf_time_cal_s)) == 0);
    }

  pdf_time_deinit (&time1);
}
END_TEST

/*
 * Test: pdf_time_get_utc_cal_002
 * Description:
 *   Create new pdf_time_t and initialize it with
 *   28857600 seconds that is 1970-12-01 in utc.
 *   Returned calendar schould be expected one.
 * Success condition:
 *   1. Function pdf_time_new schould return PDF_OK
 *   2. Function pdf_time_set_from_u32 schould return
 *      PDF_OK.
 *   3. Function pdf_time_get_utc_cal schould return
 *      PDF_OK.
 *   4. Returned localcal must be expected one.
 */
START_TEST (pdf_time_get_utc_cal_002)
{
  pdf_time_t time1;
  struct pdf_time_cal_s utccal;
  struct pdf_time_cal_s expected_cal;

  pdf_time_init (&time1);
  pdf_time_set_utc (&time1, 28857600);

  expected_cal.year = 1970;
  expected_cal.month = 12;
  expected_cal.day = 1;
  expected_cal.dow = 2;
  expected_cal.hour = 0;
  expected_cal.minute = 0;
  expected_cal.second = 0;
  expected_cal.gmt_offset = 0;

  pdf_time_get_utc_cal (&time1, &utccal);
  fail_unless (memcmp (&utccal, &expected_cal, sizeof (struct pdf_time_cal_s)) == 0);

  pdf_time_deinit (&time1);
}
END_TEST

/*
 * Test case creation function
 */
TCase *
test_pdf_time_get_utc_cal (void)
{
  TCase *tc = tcase_create ("pdf_time_get_utc_cal");

  tcase_add_test (tc, pdf_time_get_utc_cal_001);
  tcase_add_test (tc, pdf_time_get_utc_cal_002);
  tcase_add_checked_fixture (tc,
                             pdf_test_setup,
                             pdf_test_teardown);
  return tc;
}

/* End of pdf-time-get-utc-cal.c */
