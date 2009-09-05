/* -*- mode: C -*- Time-stamp: "09/09/05 22:55:26 jemarch"
 *
 *       File:         pdf-time.c
 *       Date:         Mon Apr 28 23:23:04 2008
 *
 *       GNU PDF Library - Time Module source
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

#include <time.h>

#include <pdf-alloc.h>
#include <pdf-types.h>
#include <pdf-time-context.h>
#include <pdf-time-string.h>
#include <pdf-time.h>


#define PDF_SECS_PER_DAY      86400
#define PDF_SECS_PER_HOUR      3600
#define PDF_SECS_PER_MIN         60
#define PDF_MINS_PER_HOUR        60
#define PDF_HOURS_PER_DAY        24
#define PDF_MINS_PER_DAY       1440
#define PDF_DAYS_IN_YEAR        365
#define PDF_DAYS_IN_LEAP_YEAR   366
#define PDF_MINIMUM_YEAR       1970

enum pdf_time_cal_type_e {
  PDF_TIME_CAL_LOCAL,
  PDF_TIME_CAL_UTC
};

enum pdf_time_month_e {
  PDF_TIME_JANUARY    = 1,
  PDF_TIME_FEBRUARY   = 2,
  PDF_TIME_MARCH      = 3,
  PDF_TIME_APRIL      = 4,
  PDF_TIME_MAY        = 5,
  PDF_TIME_JUNE       = 6,
  PDF_TIME_JULY       = 7,
  PDF_TIME_AUGUST     = 8,
  PDF_TIME_SEPTEMBER  = 9,
  PDF_TIME_OCTOBER    = 10,
  PDF_TIME_NOVEMBER   = 11,
  PDF_TIME_DECEMBER   = 12,
  PDF_TIME_NMONTHS
};

/* Returns PDF_TRUE if the Olimpic Games [were/will be] celebrated in the given
 *  year */
static pdf_bool_t
pdf_time_is_leap_year_p(pdf_u32_t year)
{
  /* A leap year is divisable by 4, but not by 100, except if divisable by
   *  400 */
  return (((year % 4 == 0) && \
           ((year % 100 != 0) || \
            (year % 400 == 0))) ? \
          PDF_TRUE : PDF_FALSE);
}


/* Returns number of days in the given month */
static pdf_u32_t
pdf_time_get_days_in_month(const pdf_u32_t year,
                           const enum pdf_time_month_e month)
{
  switch(month)
  {
    case PDF_TIME_JANUARY:
    case PDF_TIME_MARCH:
    case PDF_TIME_MAY:
    case PDF_TIME_JULY:
    case PDF_TIME_AUGUST:
    case PDF_TIME_OCTOBER:
    case PDF_TIME_DECEMBER:
      return 31;
    case PDF_TIME_APRIL:
    case PDF_TIME_JUNE:
    case PDF_TIME_SEPTEMBER:
    case PDF_TIME_NOVEMBER:
      return 30;
    case PDF_TIME_FEBRUARY:
      return (pdf_time_is_leap_year_p(year) ? 29 : 28);
    default:
      return 0;
  }
}


/* Returns number of days before the given month */
static pdf_u32_t
pdf_time_get_days_before_month(const pdf_u32_t year,
                               const enum pdf_time_month_e month)
{
  enum pdf_time_month_e walk = PDF_TIME_JANUARY;
  pdf_u32_t sum = 0;
  while(walk != month) {
    sum += pdf_time_get_days_in_month(year, walk++);
  }
  return sum;
}

static pdf_bool_t
pdf_time_is_valid_cal_p(const struct pdf_time_cal_s *p_cal_time)
{
/*
  PDF_DEBUG_BASE("Calendar: %d/%d/%d %d:%d:%d off:%d",
                 p_cal_time->year,
                 p_cal_time->month,
                 p_cal_time->day,
                 p_cal_time->hour,
                 p_cal_time->minute,
                 p_cal_time->second,
                 p_cal_time->gmt_offset);
*/  
  return ( ( (p_cal_time == NULL) || \
             (p_cal_time->year < PDF_MINIMUM_YEAR) || \
             (p_cal_time->month < PDF_TIME_JANUARY) || \
             (p_cal_time->month > PDF_TIME_DECEMBER) || \
             (p_cal_time->day == 0) || \
             (p_cal_time->day > pdf_time_get_days_in_month(p_cal_time->year,
                                                           p_cal_time->month)) || \
             (p_cal_time->hour >= PDF_HOURS_PER_DAY) || \
             (p_cal_time->minute >= PDF_MINS_PER_HOUR) || \
             (p_cal_time->second >= PDF_SECS_PER_MIN) ) ? PDF_FALSE : PDF_TRUE);
  
}


/* Get Break-Down calendar from pdf_time_t */
static pdf_status_t
pdf_time_get_cal (const pdf_time_t time_var,
                  const enum pdf_time_cal_type_e cal_type,
                  struct pdf_time_cal_s *p_cal_time)
{
  /* Based on glibc's __offtime function */

  pdf_i32_t days;
  pdf_i64_t aux64;
  pdf_i32_t remaining;
  pdf_i32_t years;
  pdf_i32_t months;
  pdf_time_t new_time_var;
  pdf_status_t p_status = PDF_OK;
  
  /* Duplicate time var */
  new_time_var = pdf_time_dup(time_var);

  /* pdf_time_t always stores the date & time in UTC timescale, so we only need
   *  to modify the time_var IF gmt_offset is not zero, in order to move the
   *  date & time from UTC to local time specified by the offset */
  if( (cal_type == PDF_TIME_CAL_LOCAL) && \
      (time_var->gmt_offset != 0) )
    {
      pdf_time_span_t delta;
      /* Modify time in the time object */
      delta = pdf_time_span_new();
      pdf_time_span_set_from_i32(&delta, time_var->gmt_offset);
      pdf_time_add_span(new_time_var, delta);
      pdf_time_span_destroy(&delta);
    }
  
  aux64 = pdf_i64_new(0,0);


  /* Get date as days */
  pdf_i64_div_i32_divisor(&aux64, new_time_var->seconds, PDF_SECS_PER_DAY, &p_status);
  days = pdf_i64_to_i32(aux64);
  /* Get time in seconds */
  pdf_i64_mod_i32_divisor(&aux64, new_time_var->seconds, PDF_SECS_PER_DAY, &p_status);
  remaining = pdf_i64_to_i32(aux64);
  /* Get hours */
  p_cal_time->hour = remaining / PDF_SECS_PER_HOUR;
  /* Get remaining */
  remaining = remaining % PDF_SECS_PER_HOUR;
  /* Get minutes */
  p_cal_time->minute = remaining / PDF_MINS_PER_HOUR;
  /* Get seconds */
  p_cal_time->second = remaining % PDF_MINS_PER_HOUR;
  
  /* Seems that Unix origin time was thursday */
  p_cal_time->dow = ((days+4)%7);
    
  years = 1970;


  while((days < 0) || \
        (days >= (pdf_time_is_leap_year_p(years) ? \
                  (PDF_DAYS_IN_YEAR+1) :  \
                  (PDF_DAYS_IN_YEAR))))
    {
      pdf_i32_t yg;
      yg = years;

      /* Compute number of years (assuming all years of 365 days) between the
       *  origin and our date */
      yg += (days / PDF_DAYS_IN_YEAR);
      /* Get number of remaining days after having added the fixed-size years
         If the number of remaining days is less than zero, go down 1 year */
      yg -= ((days % PDF_DAYS_IN_YEAR) < 0);


#define LEAPS_THRU_END_OF(y) ((y) / 4 - (y) / 100 + (y) / 400)
      /* Remove number of days due to the leap years */
      days -= (((yg - years)*PDF_DAYS_IN_YEAR) +	\
               (LEAPS_THRU_END_OF (yg - 1)) - \
	       (LEAPS_THRU_END_OF (years - 1)));
      years = yg;
    }

  /* Set year */
  p_cal_time->year = years;// - 1900;
  
  for (months = 11; \
       days < pdf_time_get_days_before_month(p_cal_time->year,months); \
       --months)
    continue;

  days -= pdf_time_get_days_before_month(p_cal_time->year,months);

  /* Set month and day of month */
  p_cal_time->month = months;
  if (pdf_time_get_days_in_month(p_cal_time->year, p_cal_time->month) < days + 1){
    p_cal_time->day = days + 1 - pdf_time_get_days_in_month(p_cal_time->year, p_cal_time->month);
    p_cal_time->month+=1;
    if (p_cal_time->month == 13) {
        p_cal_time->month = 1;
        p_cal_time->year+=1;
    }
  }
  else 
  p_cal_time->day = days + 1;
  
  /* Finally, set gmt offset */
  p_cal_time->gmt_offset = new_time_var->gmt_offset;
  
  pdf_time_destroy(new_time_var);
  
  return (pdf_time_is_valid_cal_p(p_cal_time) ? PDF_OK : PDF_ERROR);
}



/* Function to normalize a given date after having added YEARS */
static void
pdf_time_calendar_add_years(struct pdf_time_cal_s *p_calendar,
                            const pdf_i32_t delta_years)
{
  /* ADD years */
  p_calendar->year += delta_years;

  /* The only thing to normalize is in case we reach Feb.29 in a non-leap
   *  year */
  if( (!pdf_time_is_leap_year_p(p_calendar->year)) && \
       (p_calendar->month == PDF_TIME_FEBRUARY) && \
       (p_calendar->day == 29) )
    {
      p_calendar->day = 28;
    }
}

/* Function to normalize a given date after having added MONTHS */
static void
pdf_time_calendar_add_months(struct pdf_time_cal_s *p_calendar,
                             const pdf_i32_t delta_months)
{
  if(delta_months > 0)
    {
      p_calendar->month += delta_months;
      while(p_calendar->month > 12)
        {
          p_calendar->month -= 12;
          p_calendar->year++;
        }
    }
  else if(delta_months < 0)
    {
      p_calendar->month += delta_months;
      while(p_calendar->month < 1)
        {
          p_calendar->month += 12;
          p_calendar->year--;
        }
    }
  
  /* After having added months, we could need to normalize the days */
  if(pdf_time_get_days_in_month(p_calendar->year,
                                p_calendar->month) < p_calendar->day)
    {
      p_calendar->day = pdf_time_get_days_in_month(p_calendar->year,
                                                   p_calendar->month);
    }
}

/* Function to normalize a given date after having added DAYS */
static void
pdf_time_calendar_add_days(struct pdf_time_cal_s *p_calendar,
                           const pdf_i32_t delta_days)
{
  pdf_i32_t delta = delta_days;

  /* ADD days */
  if(delta_days > 0)
    {
      pdf_i32_t days_in_month;
  
      /* Initialize days in month */
      days_in_month = pdf_time_get_days_in_month(p_calendar->year, \
                                                 (enum pdf_time_month_e)p_calendar->month);
      while(delta > (days_in_month - p_calendar->day))
        {
          /* Go to start of next month */
          p_calendar->day = 1;
          pdf_time_calendar_add_months(p_calendar, 1);

          /* Update remaining delta and new days_in_month */
          delta -= (days_in_month - p_calendar->day +1);
          days_in_month = pdf_time_get_days_in_month(p_calendar->year, \
                                                     (enum pdf_time_month_e)p_calendar->month);
        }
      /* Add final delta, which doesn't require month change */
      p_calendar->day += delta;
    }
  
  /* SUBSTRACT days */
  else if(delta_days < 0)
    {
      pdf_i32_t days_in_month;
      
      /* Initialize days in month */
      days_in_month = p_calendar->day - 1;

      while(days_in_month <= (-delta))
        {
          /* Go to the begin of previous month */
          p_calendar->day = 1;
          pdf_time_calendar_add_months(p_calendar, -1);
          
          /* Update remaining delta and new days_in_month */
          delta += days_in_month;
          days_in_month = pdf_time_get_days_in_month(p_calendar->year, \
                                                     (enum pdf_time_month_e)p_calendar->month);
        }
      /* Add final delta, which doesn't require month change */
      delta += days_in_month;
      p_calendar->day += delta;
    }
}

/* Function to normalize a given date after having added HOURS */
static void
pdf_time_calendar_add_hours(struct pdf_time_cal_s *p_calendar,
                            const pdf_i32_t delta_hours)
{
  pdf_i32_t days;
  pdf_i32_t remaining_hours;

  /* No real problem with hours, as 1 day is always 24h */
  days = delta_hours / PDF_HOURS_PER_DAY;
  remaining_hours = delta_hours % PDF_HOURS_PER_DAY;

  /* Add remaining hours */
  p_calendar->hour += remaining_hours;
  /* If we went back to the previous day, correct time and add 1 day more
   * to remove */
  if(p_calendar->hour < 0)
    {
      p_calendar->hour += PDF_HOURS_PER_DAY;
      days--;
    }
  /* If we went forward to the next day, correct time and add 1 day more
   * to add */
  else if(p_calendar->hour >= PDF_HOURS_PER_DAY)
    {
      p_calendar->hour -= PDF_HOURS_PER_DAY;
      days++;
    }

  /* Add/Remove days... */
  pdf_time_calendar_add_days(p_calendar, days);
}

/* Function to normalize a given date after having added MINUTES */
static void
pdf_time_calendar_add_minutes(struct pdf_time_cal_s *p_calendar,
                              const pdf_i32_t delta_minutes)
{
  pdf_i32_t hours;
  pdf_i32_t remaining_minutes;
  
  /* No real problem with minutes, as 1 hour is always 60minutes */
  hours = delta_minutes / PDF_MINS_PER_HOUR;
  remaining_minutes = delta_minutes % PDF_MINS_PER_HOUR;

  /* Add remaining minutes */
  p_calendar->minute += remaining_minutes;

  /* If we went back to the previous hour, correct time and add 1 hour more
   * to remove */
  if(p_calendar->minute < 0)
    {
      p_calendar->minute += PDF_MINS_PER_HOUR;
      hours--;
    }
  /* If we went forward to the next day, correct time and add 1 hour more
   * to add */
  else if(p_calendar->minute >= PDF_MINS_PER_HOUR)
    {
      p_calendar->minute -= PDF_MINS_PER_HOUR;
      hours++;
    }
  
  /* Add/Remove hours... */
  pdf_time_calendar_add_hours(p_calendar, hours);
}


/* Function to normalize a given date after having added SECONDS */
static void
pdf_time_calendar_add_seconds(struct pdf_time_cal_s *p_calendar,
                              const pdf_i32_t delta_seconds)
{
  pdf_i32_t minutes;
  pdf_i32_t remaining_seconds;
  
  /* No real problem with minutes, as 1 hour is always 60minutes */
  minutes = delta_seconds / PDF_SECS_PER_MIN;
  remaining_seconds = delta_seconds % PDF_SECS_PER_MIN;
  
  /* Add remaining seconds */
  p_calendar->second += remaining_seconds;
  
  /* If we went back to the previous minute, correct time and add 1 minute more
   * to remove */
  if(p_calendar->second < 0)
    {
      p_calendar->second += PDF_SECS_PER_MIN;
      minutes--;
    }
  /* If we went forward to the next minute, correct time and add 1 minute more
   * to add */
  else if(p_calendar->second >= PDF_SECS_PER_MIN)
    {
      p_calendar->second -= PDF_SECS_PER_MIN;
      minutes++;
    }
  
  /* Add/Remove minutes... */
  pdf_time_calendar_add_minutes(p_calendar, minutes);
}






/* --------------------- Time Module Initialization ------------------------- */

/* Initialize Time module. Warning! Not thread-safe, must be used only once when
 *  the program starts. It will be in charge of detecting currently configured
 *  time zone (GMT offset) and so on. */
pdf_status_t
pdf_time_init(void)
{
  /* Start Time context */
  return pdf_time_context_init();
}


/* --------------------- Time Creation and Destruction ---------------------- */

/* Create new pdf_time_t object */
pdf_status_t
pdf_time_new (pdf_time_t *element)
{
  /* Allocate object & Initialize all contents */
  *element = (pdf_time_t) pdf_alloc (sizeof(struct pdf_time_s));
  if (*element == NULL)
    {
      /* Not enough memory condition */
      return PDF_ENOMEM;
    }

  /* Create pdf_i64_t */
  (*element)->seconds = pdf_i64_new(0,0);
  pdf_time_clear(*element);

  /* Success */
  return PDF_OK;
}

/* Duplicate pdf_time_t object */
pdf_time_t
pdf_time_dup (const pdf_time_t orig)
{
  pdf_time_t element;

  if((pdf_time_new (&element) == PDF_OK) &&
     (pdf_time_copy(orig, element) != PDF_OK))
    {
      pdf_time_destroy(element);
      element = NULL;
    }

  return element;
}


/* Destroy pdf_time_t object */
pdf_status_t
pdf_time_destroy (pdf_time_t time_var)
{
  pdf_time_clear(time_var); /* Just in case additional contents are stored */
  pdf_dealloc(time_var);
  return PDF_OK;
}


/* Set time value with a 32-bit unsigned integer */
pdf_status_t
pdf_time_set_from_u32 (pdf_time_t time_var,
                       pdf_u32_t seconds)
{
  pdf_status_t ret_code;
  pdf_i64_assign(&(time_var->seconds), 0, seconds, &ret_code);
  return ret_code;  
}


/* Set time value with a pdf_i64_t variable */
pdf_status_t
pdf_time_set_from_i64 (pdf_time_t time_var,
                       pdf_i64_t  seconds)
{
  pdf_status_t ret_code;
  pdf_i64_copy(seconds, &(time_var->seconds), &ret_code);
  return ret_code;
}

#ifdef PDF_HOST_WIN32
/* Windows-specific function to set the time with a FILETIME structure */
pdf_status_t
pdf_time_w32_set_from_filetime (pdf_time_t time_var,
                                const FILETIME *p_filetime)
{
  SYSTEMTIME systemtime;

  
  /*
    BOOL WINAPI
    FileTimeToSystemTime(const FILETIME *lpFileTime,
                         LPSYSTEMTIME lpSystemTime);

    If the function succeeds, the return value is nonzero.

    typedef struct _SYSTEMTIME {
      WORD wYear;
      WORD wMonth;        (1: january, 2: february...)
      WORD wDayOfWeek;    (0:sunday, 1: monday...)
      WORD wDay;
      WORD wHour;
      WORD wMinute;
      WORD wSecond;
      WORD wMilliseconds;
    } SYSTEMTIME;

   */

  /* Convert filetime to systemtime calendar */
  if(FileTimeToSystemTime(p_filetime,&systemtime)==0)
    {
      PDF_DEBUG_BASE("FileTimeToSystemTime failed...");
      return PDF_ERROR;
    }
  else
    {
      /* Convert from systemtime calendar to our pdf_time_cal_s */
      struct pdf_time_cal_s calendar;

      calendar.year = systemtime.wYear;
      calendar.month = systemtime.wMonth;
      calendar.day = systemtime.wDay;
      calendar.hour = systemtime.wHour;
      calendar.minute = systemtime.wMinute;
      calendar.second = systemtime.wSecond;
      /* Round seconds if milliseconds available */
      if(systemtime.wMilliseconds >= 500)
        {
          calendar.second++;
        }
      calendar.gmt_offset = 0;
      /* For us, sunday is 7 */
      calendar.dow = (systemtime.wDayOfWeek == 0) ? 7: systemtime.wDayOfWeek;
      
      return pdf_time_from_cal(time_var,&calendar);
    }
}
#endif





/* ------------------------- Managing Time Values --------------------------- */

/* Copy the contents of a given pdf_time_t object */
pdf_status_t
pdf_time_copy (const pdf_time_t orig,
               pdf_time_t copy)
{
  pdf_status_t p_status = PDF_OK;
  pdf_i64_copy (orig->seconds, &(copy->seconds), &p_status);
  copy->gmt_offset = orig->gmt_offset;
  return p_status;
}

/* Clear contents of the pdf_time_t object */
pdf_status_t
pdf_time_clear (pdf_time_t time_var)
{
  pdf_status_t p_status = PDF_OK;
  /* Set time as January 1st, 1970 */
  pdf_i64_assign_quick(&time_var->seconds, 0, &p_status);
  /* UTC */
  time_var->gmt_offset = 0;
  return p_status;
}



/*
*  Based on the work done by Perl guys in DateTime:
*  http://search.cpan.org/dist/DateTime/lib/DateTime.pm#Adding_a_Duration_to_a_Datetime
*
*  "DateTime.pm always adds (or subtracts) days, then months, minutes, and then
*   seconds and nanoseconds. If there are any boundary overflows, these are
*   normalized at each step. For the days and months (the calendar) the local
*   (not UTC) values are used. For minutes and seconds, the local values are
*   used. This generally just works. 
*
*   This means that adding one month and one day to February 28, 2003 will
*   produce the date April 1, 2003, not March 29, 2003."
*
* Thanks to S.Jansen for the link!
*/
static pdf_status_t
pdf_time_add_cal_span_with_sign (pdf_time_t time_var,
                                 const struct pdf_time_cal_span_s *p_cal_span,
                                 int sign)
{
  pdf_status_t status = PDF_ERROR;

  /* Check allowed sign values */
  if( (sign == -1) || \
      (sign == 1) )
    {
      struct pdf_time_cal_s calendar;
      
      /* Create Calendar type from the time object */
      if(pdf_time_get_utc_cal (time_var, &calendar) == PDF_OK)
        {
          pdf_time_calendar_add_days(&calendar,     sign * p_cal_span->days);
          pdf_time_calendar_add_months(&calendar,   sign * p_cal_span->months);
          pdf_time_calendar_add_years(&calendar,    sign * p_cal_span->years);
          pdf_time_calendar_add_hours(&calendar,    sign * p_cal_span->hours);
          pdf_time_calendar_add_minutes(&calendar,  sign * p_cal_span->minutes);
          pdf_time_calendar_add_seconds(&calendar,  sign * p_cal_span->seconds);
          
          status = pdf_time_from_cal(time_var, &calendar);
        }
    }
  
  return status;
}

/* Add the time span represented by cal_span to the text object. */
pdf_status_t
pdf_time_add_cal_span (pdf_time_t time_var,
                       const struct pdf_time_cal_span_s *p_cal_span)
{
  return pdf_time_add_cal_span_with_sign(time_var, p_cal_span, 1);
}

/* Substract the time span represented by cal_span from the text object */
pdf_status_t
pdf_time_sub_cal_span (pdf_time_t time_var,
                       const struct pdf_time_cal_span_s *p_cal_span)
{
  return pdf_time_add_cal_span_with_sign(time_var, p_cal_span, -1);
}

/* Add the time span contained in time_span to time. As the time span is stored
 * in seconds, the adding is direct. */
pdf_status_t
pdf_time_add_span (pdf_time_t time_var,
                   const pdf_time_span_t time_span)
{
  pdf_status_t p_status = PDF_OK;
  pdf_i64_add ((&time_var->seconds),
               time_var->seconds,
               time_span, &p_status);
  return p_status;
}

/* Subtract the time span contained in time_span to time. As the time span is
 * stored in seconds. */
pdf_status_t
pdf_time_sub_span (pdf_time_t time_var,
                   const pdf_time_span_t time_span)
{
  pdf_status_t p_status = PDF_OK;
  pdf_i64_subtraction ((&time_var->seconds),
                       time_var->seconds,
                       time_span, &p_status);
  return p_status;
}


/* Fill local_cal with the local calendar time of object. */
pdf_status_t
pdf_time_get_local_cal (const pdf_time_t time_var,
                        struct pdf_time_cal_s *p_local_cal)
{
  return pdf_time_get_cal(time_var, PDF_TIME_CAL_LOCAL,
                          p_local_cal);
}

/* Get the UTC calendar time of a given time variable. */
pdf_status_t
pdf_time_get_utc_cal (const pdf_time_t time_var,
                      struct pdf_time_cal_s *p_utc_cal)
{
  return pdf_time_get_cal(time_var, PDF_TIME_CAL_UTC,
                          p_utc_cal);
}


/* Set the value of a time variable to a given calendar time. */
pdf_status_t
pdf_time_from_cal (pdf_time_t time_var,
                   const struct pdf_time_cal_s *p_cal_time)
{
  pdf_i64_t aux;
  pdf_i32_t walker;
  pdf_status_t p_status = PDF_OK;

  if( (time_var == NULL) || \
      (! pdf_time_is_valid_cal_p(p_cal_time)) )
    {
      PDF_DEBUG_BASE("Invalid arguments received");
      return PDF_EBADDATA;
    }
  
  /* Initialize days to 0 */
  aux = pdf_i64_new(0,0);

  /* Add days per year until the current year in the calendar */
  walker = 1970;
  while(walker < p_cal_time->year)
    {
      pdf_i64_add_i32(&aux, aux, \
                      (pdf_time_is_leap_year_p(walker) ? \
                       PDF_DAYS_IN_LEAP_YEAR : PDF_DAYS_IN_YEAR), &p_status);
      walker++;
    }

  /* Add days per month until the current month in the calendar */
  pdf_i64_add_i32(&aux, aux, pdf_time_get_days_before_month(p_cal_time->year,
                                                            p_cal_time->month), &p_status);

  /* Add days in current month until the current required day */
  pdf_i64_add_i32(&aux, aux, p_cal_time->day -1, &p_status);

  /* Set date as seconds in the output variable */
  pdf_i64_mult_i32(&(time_var->seconds), aux, PDF_SECS_PER_DAY, &p_status);

  /* Add hours as seconds */
  pdf_i64_add_i32(&(time_var->seconds), \
                  (time_var->seconds), \
                  p_cal_time->hour * PDF_SECS_PER_HOUR, &p_status);
  /* Add minutes as seconds */
  pdf_i64_add_i32(&(time_var->seconds), \
                  (time_var->seconds), \
                  p_cal_time->minute * PDF_SECS_PER_MIN, &p_status);
  /* Finally, add seconds */
  pdf_i64_add_i32(&(time_var->seconds), \
                  (time_var->seconds), \
                  p_cal_time->second, &p_status);
  
  /* Set specific GMT offset if any */
  if(p_cal_time->gmt_offset != 0)
    {
      /* Remove it from the time value (calendar comes in local time, so
       *  we must remove the offset to get the pdf_time_t->seconds in UTC. */
      pdf_time_span_t delta = pdf_time_span_new();
      pdf_time_span_set_from_i32(&delta, p_cal_time->gmt_offset);
      pdf_time_sub_span(time_var, delta);
      pdf_time_span_destroy(&delta);
    }

  /* Store the offset */
  time_var->gmt_offset = p_cal_time->gmt_offset;

  return PDF_OK;
}

/* Set the local time offset of time to the one used by the operating system. */
pdf_status_t
pdf_time_set_local_offset (pdf_time_t time_var)
{
  /* Set local GMT offset */
  time_var->gmt_offset = pdf_time_context_get_gmt_offset();
  return PDF_OK;
}


/* ----------------------- Getting Time Intervals --------------------------- */

/* Get time interval as Calendar Span */
pdf_status_t
pdf_time_diff_cal (const pdf_time_t time1,
                   const pdf_time_t time2,
                   struct pdf_time_cal_span_s *p_cal_span)
{
  pdf_status_t ret_code = PDF_ERROR;
  struct pdf_time_cal_s calendar1;
  struct pdf_time_cal_s calendar2;

  if( (p_cal_span != NULL) && \
      (pdf_time_get_cal(time1,
                        PDF_TIME_CAL_UTC,
                        &calendar1) == PDF_OK) && \
      (pdf_time_get_cal(time2,
                        PDF_TIME_CAL_UTC,
                        &calendar2) == PDF_OK) )
    {
      struct pdf_time_cal_s *p_big;
      struct pdf_time_cal_s *p_small;
      pdf_i32_t aux;

      /* Check which of the dates is bigger */
      if(pdf_time_cmp(time1, time2) > 0)
        {
          p_cal_span->sign = PDF_TRUE;
          p_big = &calendar1;
          p_small = &calendar2;
        }
      else
        {
          p_cal_span->sign = PDF_FALSE;
          p_big = &calendar2;
          p_small = &calendar1;          
        }
      
      /* Get diff of years directly (always + or 0) */
      p_cal_span->years = p_big->year - p_small->year;

#define DIFF_AND_CORRECT(field,spanfield,spanupperfield) \
  do { \
      aux = p_big->field - p_small->field; \
      if(aux < 0) { \
          p_cal_span->spanupperfield--; \
          p_cal_span->spanfield = (-1)*aux; \
      } else { \
        p_cal_span->spanfield = aux; \
      } \
  } while(0)

      /* Get diff of months (could be -) */
      DIFF_AND_CORRECT(month,   months,   years);
      /* Get diff of days (could be -) */
      DIFF_AND_CORRECT(day,     days,     months);
      /* Get diff of hours (could be -) */
      DIFF_AND_CORRECT(hour,    hours,    days);
      /* Get diff of minutes (could be -) */
      DIFF_AND_CORRECT(minute,  minutes,  hours);
      /* Get diff of seconds (could be -) */
      DIFF_AND_CORRECT(second,  seconds,  minutes);

#undef DIFF_AND_CORRECT

      ret_code = PDF_OK;
    }

  return ret_code;
}

/* Get time interval as Span */
pdf_status_t
pdf_time_diff (const pdf_time_t time1,
               const pdf_time_t time2,
               pdf_time_span_t  *p_time_span)
{ 
  pdf_status_t p_status = PDF_OK;
  pdf_i64_subtraction(p_time_span, time1->seconds, time2->seconds, &p_status);
  return p_status;
}


/* ---------------------------- Time comparison ----------------------------- */

/* Compares two times. */
pdf_i32_t
pdf_time_cmp (const pdf_time_t time1,
              const pdf_time_t time2)
{
  return (pdf_i32_t)pdf_i64_cmp(time1->seconds, time2->seconds);
}


/* ---------------------- Time printing and parsing ------------------------- */

/* Create a string representation of a given time. */
pdf_char_t *
pdf_time_to_string (const pdf_time_t time_var,
                    const enum pdf_time_format_e time_format,
                    pdf_bool_t include_trailing_apostrophe)
{
  switch(time_format)
  {
    case PDF_TIME_FORMAT_PDF:
      return pdf_time_to_string_pdf (time_var,
                                     include_trailing_apostrophe);
    case PDF_TIME_FORMAT_ISO_8601:
      return pdf_time_to_string_iso8601(time_var);
    case PDF_TIME_FORMAT_UTC_ASN1:
      return pdf_time_to_string_utc_asn1(time_var);
    case PDF_TIME_FORMAT_GENERALIZED_ASN1:
      return pdf_time_to_string_generalized_asn1(time_var);
    default:
      return NULL;
  }
}



/* Get a string containing a time specification in some format and fill a time
 *  variable with the parsed values. */
pdf_status_t
pdf_time_from_string (pdf_time_t time_var,
                      const pdf_char_t *time_str,
                      const enum pdf_time_format_e time_format,
                      pdf_bool_t require_trailing_apostrophe)
{
  switch(time_format)
    {
      case PDF_TIME_FORMAT_PDF:
        return pdf_time_from_string_pdf(time_var, time_str, require_trailing_apostrophe);
      case PDF_TIME_FORMAT_ISO_8601:
        return pdf_time_from_string_iso8601(time_var, time_str);
      case PDF_TIME_FORMAT_UTC_ASN1:
        return pdf_time_from_string_utc_asn1(time_var, time_str);
      case PDF_TIME_FORMAT_GENERALIZED_ASN1:
        return pdf_time_from_string_generalized_asn1(time_var, time_str);
      default:
        return PDF_ERROR;
    }
}


/* ---------------------- Getting the Current Time -------------------------- */

/* Set the value of object to the current local time used by the operating
 *  system. */
pdf_status_t
pdf_time_set_to_current_local_time (pdf_time_t time_var)
{
  if(pdf_time_set_to_current_utc_time(time_var) == PDF_OK)
    {
      /* And store offset in the gmt_offset */
      time_var->gmt_offset = pdf_time_context_get_gmt_offset();
      return PDF_OK;
    }
  else
    {
      return PDF_ERROR;
    }
}


/* Set the value of object to the current UTC time used by the operating
 *  system. */
pdf_status_t
pdf_time_set_to_current_utc_time (pdf_time_t time_var)
{
  pdf_status_t p_status = PDF_OK;
  time_t time_now = time(NULL);
  if(time_now != -1)
    {
      /* At least until 2038 this call will work correctly, even in systems with
       *  a 32bit time_t */
      pdf_i64_assign_quick(&(time_var->seconds), (pdf_i32_t)time_now, &p_status);
      time_var->gmt_offset = 0;
    }
  return p_status;
}


/* ----------------- Time Span Creation and Destruction --------------------- */
/* Note: It seems quite weird to manage the time span as any other pdf_X object,
 *  taking into account that it's just a typedef of pdf_i64_t, BUT, keeping the
 *  common interface would allow easy updates in the future if needed */

/* Create new time span object */
pdf_time_span_t
pdf_time_span_new (void)
{
  return pdf_i64_new(0,0);
}


/* Duplicate time span object */
pdf_time_span_t
pdf_time_span_dup (const pdf_time_span_t span)
{
  pdf_time_span_t new_span;
  new_span = pdf_time_span_new();
  pdf_time_span_copy(span, &new_span);
  return new_span;
}


/* Destroy time span object */
pdf_status_t
pdf_time_span_destroy (pdf_time_span_t *p_span)
{
  return PDF_OK;
}


/* --------------------- Managing Time Span Values -------------------------- */

/* Set the value of a time span from a 64 bits signed number */
pdf_status_t
pdf_time_span_set (pdf_time_span_t *p_span,
                   const pdf_i32_t high_value,
                   const pdf_u32_t low_value)
{
  pdf_status_t p_status = PDF_OK;
  pdf_i64_assign(p_span, high_value, low_value, &p_status);
  return p_status;
}


/* Set the value of a time span from a 32 bits signed number. */
pdf_status_t
pdf_time_span_set_from_i32 (pdf_time_span_t *p_span,
                            const pdf_i32_t seconds)
{
  pdf_status_t p_status = PDF_OK;
  pdf_i64_assign_quick(p_span, seconds, &p_status);
  return p_status;
}


/* Change sign of time span */
pdf_status_t
pdf_time_span_negate (pdf_time_span_t *p_span)
{
  pdf_status_t p_status = PDF_OK;
  if (p_span == NULL)
    return PDF_ERROR;
  pdf_i64_neg(p_span, *p_span, &p_status);
  return p_status;
}


/* Add two time spans and store the result in another time span. */
pdf_status_t
pdf_time_span_add (const pdf_time_span_t span1,
                   const pdf_time_span_t span2,
                   pdf_time_span_t *p_result)
{
  pdf_status_t p_status = PDF_OK;
  pdf_i64_add((pdf_i64_t *)p_result, span1, span2, &p_status);
  return p_status;
}

/* Copy the value of a time span into another time span. */
pdf_status_t
pdf_time_span_copy (const pdf_time_span_t orig,
                    pdf_time_span_t *p_dest)
{
  pdf_status_t p_status = PDF_OK;
  pdf_i64_copy(orig, (pdf_i64_t *)p_dest, &p_status);
  return p_status;
}

/* Difference two time spans and store the result (maybe negative) into another
 *  time span. */
pdf_status_t
pdf_time_span_diff (const pdf_time_span_t span1,
                    const pdf_time_span_t span2,
                    pdf_time_span_t *p_result)
{
  pdf_status_t p_status= PDF_OK;
  pdf_i64_subtraction(p_result, span1, span2, &p_status);
  return p_status;
}

/* Get the value of a time span in seconds. */
pdf_i64_t
pdf_time_span_to_secs (const pdf_time_span_t span)
{
  return span;
}


/* ------------------------- Time Span Comparison --------------------------- */

/* Compare two time spans */
pdf_i32_t
pdf_time_span_cmp (const pdf_time_span_t span1,
                   const pdf_time_span_t span2)
{
  return (pdf_i64_cmp(span1, span2));
}


/* ---------------------- Calendar Spans Management ------------------------- */



static pdf_status_t
pdf_time_span_from_cal_span(pdf_time_span_t *p_span,
                            const struct pdf_time_cal_span_s *p_cal_span,
                            const pdf_time_t base_time)
{
  pdf_status_t ret_code = PDF_ERROR;
  pdf_time_t new_time = NULL;

  /* Duplicate base object and add calendar span */
  new_time = pdf_time_dup(base_time);
  if( (new_time != NULL) &&
      (pdf_time_add_cal_span(new_time,p_cal_span) == PDF_OK) )
    {
      /* Get the difference in seconds */
      ret_code = pdf_time_diff(new_time, base_time, p_span);
      pdf_time_destroy(new_time);
      new_time = NULL;
    }

  /* Destroy allocated object, if any */
  if(new_time != NULL)
    {
      pdf_time_destroy(new_time);
    }
  return ret_code;
}


static pdf_status_t
pdf_time_span_to_cal_span(struct pdf_time_cal_span_s *p_cal_span,
                          const pdf_time_span_t span,
                          const pdf_time_t base_time)
{
  pdf_status_t ret_code = PDF_ERROR;
  pdf_time_t new_time = NULL;
  
  /* Duplicate base object and add calendar span,
   *  and get both initial and new times as calendars
   */
  new_time = pdf_time_dup(base_time);
  if((new_time != NULL) && \
     (pdf_time_add_span(new_time,span) == PDF_OK))
    {
      /* Now, directly get calendar diff */
      ret_code = pdf_time_diff_cal(base_time, new_time, p_cal_span);
    }

  /* Destroy allocated object, if any */
  if(new_time != NULL)
    {
      pdf_time_destroy(new_time);
    }
  return ret_code; 
}

/* Add two calendar spans. Since the calendar spans are relative (some years
 *  has more days than another) the calendar spans are first resolved from a
 *  base time to get the number of seconds, and then that number is stored in
 *  the resulting calendar span */
pdf_status_t
pdf_time_add_cal_span_with_base (const struct pdf_time_cal_span_s *p_span1,
                                 const struct pdf_time_cal_span_s *p_span2,
                                 const pdf_time_t base_time,
                                 struct pdf_time_cal_span_s *p_result)
{
  pdf_status_t  ret_code = PDF_ERROR;
  pdf_time_span_t span_time_1;
  pdf_time_span_t span_time_2;
  
  span_time_1 = pdf_time_span_new();
  span_time_2 = pdf_time_span_new();
  
  if( (p_span1 != NULL) && \
      (p_span2 != NULL) && \
      (pdf_time_span_from_cal_span(&span_time_1,
                                   p_span1,
                                   base_time) == PDF_OK) && \
      (pdf_time_span_from_cal_span(&span_time_2,
                                  p_span2,
                                  base_time) == PDF_OK) )
    {
      /* Now add two time spans */
      pdf_time_span_add(span_time_1, span_time_2, &span_time_1);

      /* Get calendar span from the new result */
      ret_code = pdf_time_span_to_cal_span(p_result,
                                           span_time_1,
                                           base_time);
    }

  pdf_time_span_destroy(&span_time_1);
  pdf_time_span_destroy(&span_time_2);
  return ret_code;
}

/* Compare two calendar spans previously resolved with a given base time. */
pdf_i32_t
pdf_time_cal_span_cmp (const struct pdf_time_cal_span_s *p_span1,
                       const struct pdf_time_cal_span_s *p_span2,
                       const pdf_time_t base_time,
                       pdf_status_t *p_ret_code)
{
  /* Probably the best way to do it is convert the cal spans into pdf_time_t
   * and then compare the pdf_time_ts */
  pdf_time_t time1;
  pdf_time_t time2;
  pdf_i32_t cmp_ret = 0;
  pdf_status_t  ret_code = PDF_ERROR;
  
  time1 = pdf_time_dup(base_time);
  time2 = pdf_time_dup(base_time);
  
  if( (time1 != NULL) && \
      (time2 != NULL) && \
      (p_span1 != NULL) && \
      (p_span2 != NULL) && \
      (pdf_time_add_cal_span(time1, p_span1) == PDF_OK) && \
      (pdf_time_add_cal_span(time2, p_span2) == PDF_OK) )
    {
      cmp_ret = pdf_time_cmp(time1, time2);
      ret_code = PDF_OK;
    }
  
  if(p_ret_code != NULL)
    {
      *p_ret_code = ret_code;
    }

  pdf_time_destroy(time1);
  pdf_time_destroy(time2);
  return cmp_ret;
}

/* Compute the difference between two calendar spans relative to a given base
 *  time and store it in a given calendar span. */
pdf_status_t
pdf_time_cal_span_diff (const struct pdf_time_cal_span_s *p_span1,
                        const struct pdf_time_cal_span_s *p_span2,
                        const pdf_time_t base_time,
                        struct pdf_time_cal_span_s *p_result)
{
  pdf_status_t  ret_code = PDF_ERROR;
  pdf_time_span_t span_time_1;
  pdf_time_span_t span_time_2;
  
  span_time_1 = pdf_time_span_new();
  span_time_2 = pdf_time_span_new();
  
  if((p_span1 != NULL) && \
     (p_span2 != NULL) && \
     (pdf_time_span_from_cal_span(&span_time_1,
                                  p_span1,
                                  base_time) == PDF_OK) && \
     (pdf_time_span_from_cal_span(&span_time_2,
                                  p_span2,
                                  base_time) == PDF_OK) )
    {
      /* Now substract the two time spans */
      pdf_time_span_diff(span_time_1, span_time_2, &span_time_1);
      
      /* Get calendar span from the new result */
      ret_code = pdf_time_span_to_cal_span(p_result,
                                           span_time_1,
                                           base_time);
    }
  
  pdf_time_span_destroy(&span_time_1);
  pdf_time_span_destroy(&span_time_2);
  return ret_code;
}


/* End of pdf-time.c */
