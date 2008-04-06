/* -*- mode: C -*- Time-stamp: ""
 *
 *       File:         pdf-text-get-country.c
 *       Date:         Mon Mar 10 18:30:01 2008
 *
 *       GNU PDF Library - Text Module - pdf_text_get_country test case
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

/*
 * Test: pdf_text_get_country_001
 * Description:
 *   Get country ID when the stored country ID is valid
 * Success conditions:
 *   1. The call to pdf_text_get_country should return a valid pointer.
 *   2. The length of the returned string should be 2. 
 */
START_TEST(pdf_text_get_country_001)
{
  pdf_text_t text;
  const pdf_char_t *country = NULL;

  /* Always INIT! Check runs each test in a different process */
  fail_if(pdf_text_init() != PDF_OK);
  
  text = pdf_text_new();
  fail_if(text == NULL);
  
  /* Store country ID without using the API */
  strncpy((char *)(&(text->country[0])), "FR", 2);
  text->country[2] = '\0';
  
  /* Get country ID */
  country = pdf_text_get_country(text);
  
  /* 1. The call to pdf_text_get_country should return a valid pointer. */
  fail_if(country == NULL);
  
  /* 2. The length of the returned string should be 2. */
  fail_unless(strlen((char *)country) == 2);
}
END_TEST


/*
 * Test: pdf_text_get_country_002
 * Description:
 *   Get country ID when the stored country ID is invalid
 * Success conditions:
 *   1. The call to pdf_text_get_country should return a valid pointer.
 *   2. The length of the returned string should be 0. 
 */
START_TEST(pdf_text_get_country_002)
{
  pdf_text_t text;
  const pdf_char_t *country = NULL;
  
  /* Always INIT! Check runs each test in a different process */
  fail_if(pdf_text_init() != PDF_OK);

  text = pdf_text_new();
  fail_if(text == NULL);

  /* Get country ID */
  country = pdf_text_get_country(text);
  
  /* 1. The call to pdf_text_get_country should return a valid pointer. */
  fail_if(country == NULL);
  
  /* 2. The length of the returned string should be 2. */
  fail_unless(strlen((char *)country) == 0);
}
END_TEST


/*
 * Test case creation function
 */
TCase *
test_pdf_text_get_country (void)
{
  TCase *tc = tcase_create("pdf_text_get_country");
  tcase_add_test(tc, pdf_text_get_country_001);
  tcase_add_test(tc, pdf_text_get_country_002);
  return tc;
}

/* End of pdf-text-get-country.c */