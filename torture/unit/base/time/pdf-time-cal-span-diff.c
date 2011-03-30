/* -*- mode: C -*-
 *
 *       File:         pdf-time-cal-span-diff.c
 *       Date:         Fri Feb 27 17:35:31 2009
 *
 *       GNU PDF Library - Unit tests for pdf_time_cal_span_diff
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
#include <base/time/pdf-time-test-common.h>
#include <pdf-test-common.h>

/*
 * Test: pdf_time_cal_span_diff_001
 * Description:
 *   Create two pdf_time_cal_span_s objects.
 *   Init them and get  pdf_time_cal_span_diff. Next
 *   add that cal_span to cleared pdf_time_t object
 *   and compare it with other object initialized
 *   with seconds.
 *Success condition:
 * 1. Function pdf_time_cal_span_diff schould return
 * PDF_OK
 * 2. Function pdf_time_add_cal_span schould return
 * PDF_OK
 * 3. Function pdf_time_cmp schould return 0
 */
START_TEST (pdf_time_cal_span_diff_001)
{
  extern struct pdf_time_cal_span_s day_time_span[];
  extern struct pdf_time_cal_span_s years_months[];
  extern pdf_u32_t days_in_seconds[];

  pdf_u32_t i, j, i2, j2;

  for (i = 0; i < DAY_TIME_SPAN_SIZE; i++)
    {
      i2 = i;
      for (j = 0, j2 = 0 ; j < YEAR_MONTH_SIZE; j++)
        {
          pdf_i64_t sec;
          pdf_time_t basetime;
          pdf_time_t mytime;
          pdf_time_t resulttime;
          struct pdf_time_cal_span_s span1;
          struct pdf_time_cal_span_s span2;
          struct pdf_time_cal_span_s result;

          pdf_time_init (&basetime);
          pdf_time_init (&mytime);
          pdf_time_init (&resulttime);

          span1.sign = PDF_FALSE;
          span1.years = years_months[j].years;
          span1.months = years_months[j].months;
          span1.days = day_time_span[i].days;
          span1.hours = day_time_span[i].hours;
          span1.minutes = day_time_span[i].minutes;
          span1.seconds = day_time_span[i].seconds;

          span2.sign = PDF_FALSE;
          span2.years = years_months[j2].years;
          span2.months = years_months[j2].months;
          span2.days = day_time_span[i2].days;
          span2.hours = day_time_span[i2].hours;
          span2.minutes = day_time_span[i2].minutes;
          span2.seconds = day_time_span[i2].seconds;

          pdf_time_cal_span_diff (&span1, &span2, &basetime, &result);

          sec = (days_in_seconds[j] - days_in_seconds[j2]+
                 (span1.days - span2.days) * 24 * 3600 +
                 (span1.hours - span2.hours) * 3600 +
                 (span1.minutes - span2.minutes) * 60 +
                 (span1.seconds - span2.seconds));

          pdf_time_set_utc (&mytime, sec);

          pdf_time_add_cal_span (&resulttime, &result);

          fail_unless (pdf_time_cmp (&mytime, &resulttime) == 0);


          pdf_time_deinit (&basetime);
          pdf_time_deinit (&mytime);
          pdf_time_deinit (&resulttime);

          i2 = i;
          j2 = j;
        }
    }
}
END_TEST

/*
 * Test case creation function
 */
TCase *
test_pdf_time_cal_span_diff (void)
{
  TCase *tc = tcase_create ("pdf_time_cal_span_diff");

  tcase_add_test (tc, pdf_time_cal_span_diff_001);
  tcase_add_checked_fixture (tc,
                             pdf_test_setup,
                             pdf_test_teardown);
  return tc;
}

/* End of pdf-time-cal-span-diff.c */
