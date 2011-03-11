/* -*- mode: C -*-
 *
 *       File:         pdf-text-new-from-host.c
 *       Date:         Mon Mar 10 18:30:01 2008
 *
 *       GNU PDF Library - Text Module - pdf_text_new_from_host test case
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
#include <base/text/pdf-text-test-common.h>
#include <pdf-test-common.h>

/*
 * Test: pdf_text_new_from_host_001
 * Description:
 *   Create a text object with an input valid host-encoded string
 * Success conditions:
 *    1. The call to  pdf_text_new_from_host should return PDF_OK.
 *    2. The contents of the text object must be the expected ones.
 */
START_TEST (pdf_text_new_from_host_001)
{
#ifdef PDF_HOST_WIN32
  const pdf_char_t *host_encoding = "CP20127"; /* us-ascii */
#else
  const pdf_char_t *host_encoding = "us-ascii";
#endif
  extern const test_string_t ascii_strings[];
  int i;

  /* Test ASCII host encoding */
  i = 0;
  while (ascii_strings[i].data != NULL)
    {
      pdf_error_t *error = NULL;
      pdf_text_t *text;
      const pdf_char_t *input_data;
      pdf_size_t input_size;
      pdf_size_t actual_size;
      pdf_char_t *actual_data;
      pdf_char_t *expected_data;
      pdf_size_t expected_size;
      pdf_bool_t expected_free = PDF_FALSE;

      /* Set input data */
      input_data = (pdf_char_t *)ascii_strings[i].data;
      input_size = (pdf_size_t)ascii_strings[i].size;

      /* Set expected data */
      expected_data = (pdf_char_t *)ascii_strings[i].utf32be_data;
      expected_size = ascii_strings[i].utf32be_size;
      if (!pdf_text_test_big_endian_system ())
        {
          /* Must change endianness of expected_data */
          expected_free = PDF_TRUE;
          expected_data = pdf_text_test_change_utf32_endianness (expected_data,
								 expected_size);
          /* Just in case... */
          fail_if (expected_data == NULL);
        }

      /* 1. The call to  pdf_text_new_from_host should return PDF_OK. */
      text = pdf_text_new_from_host (input_data,
				     input_size,
				     host_encoding,
				     &error);
      fail_unless (text != NULL);
      fail_if (error != NULL);

      /* 2. The contents of the text object must be the expected ones. */
      actual_data = pdf_text_get_unicode (text,
					  PDF_TEXT_UTF32_HE,
					  PDF_TEXT_UNICODE_NO_OPTION,
					  &actual_size,
					  &error);
      fail_unless (actual_data != NULL);
      fail_if (error != NULL);
      fail_unless (actual_size == expected_size);
      fail_unless (memcmp (actual_data, expected_data, expected_size) == 0);

      pdf_text_destroy (text);

      if (expected_free)
	pdf_dealloc (expected_data);

      ++i;
    }
}
END_TEST

/*
 * Test: pdf_text_new_from_host_002
 * Description:
 *   Create a text object with an input invalid host-encoded string (test
 *    an UTF-8 encoded string as if it were US-ASCII)
 * Success conditions:
 *    1. The call to pdf_text_new_from_host should NOT return a valid text
 *       object.
 */
START_TEST (pdf_text_new_from_host_002)
{
#ifdef PDF_HOST_WIN32
  const pdf_char_t *host_encoding = "CP20127"; /* us-ascii */
#else
  const pdf_char_t *host_encoding = "us-ascii";
#endif
  pdf_error_t *error = NULL;
  pdf_text_t *text;
  const pdf_char_t *sample_utf8 = (pdf_char_t *)"\342\202\254"; /* EURO SIGN */

  /* 1. The call to  pdf_text_new_from_host should NOT return PDF_OK. */
  text = pdf_text_new_from_host (sample_utf8,
                                 strlen (sample_utf8),
                                 host_encoding,
                                 &error);
  fail_if (text != NULL);
  fail_unless (error != NULL);
  pdf_error_destroy (error);
}
END_TEST

/*
 * Test: pdf_text_new_from_host_003
 * Description:
 *   Create a text object with an input string encoded in an invalid host
 *   encoding
 * Success conditions:
 *    1. The call to  pdf_text_new_from_host should NOT return PDF_OK.
 */
START_TEST (pdf_text_new_from_host_003)
{
#ifdef PDF_HOST_WIN32
  const pdf_char_t *host_encoding = "CP17";
#else
  const pdf_char_t *host_encoding = "invalid_host_enc";
#endif
  pdf_error_t *error = NULL;
  pdf_text_t *text;
  const pdf_char_t *sample_usascii = (pdf_char_t *)"GNU's not Unix";

  /* 1. The call to  pdf_text_new_from_host should NOT return PDF_OK. */
  text = pdf_text_new_from_host (sample_usascii,
                                 strlen (sample_usascii),
                                 host_encoding,
                                 &error);
  fail_if (text != NULL);
  fail_unless (error != NULL);
  pdf_error_destroy (error);
}
END_TEST

/*
 * Test case creation function
 */
TCase *
test_pdf_text_new_from_host(void)
{
  TCase *tc = tcase_create("pdf_text_new_from_host");

  tcase_add_test(tc, pdf_text_new_from_host_001);
  tcase_add_test(tc, pdf_text_new_from_host_002);
  tcase_add_test(tc, pdf_text_new_from_host_003);
  tcase_add_checked_fixture (tc,
                             pdf_test_setup,
                             pdf_test_teardown);
  return tc;
}

/* End of pdf-text-new-from-host.c */
