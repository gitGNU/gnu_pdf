/* -*- mode: C -*-
 *
 *       File:         pdf-text-get-pdfdocenc.c
 *       Date:         Mon Mar 10 18:30:01 2008
 *
 *       GNU PDF Library - Text Module - pdf_text_get_pdfdocenc test case
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
 * Test: pdf_text_get_pdfdocenc_001
 * Description:
 *   Get the contents of a text object in PDF Doc Encoding. The contents of the
 *   text object can all be transformed to PDF Doc Encoding without loss of
 *   information.
 * Success conditions:
 *    1. The call to  pdf_text_get_pdfdocenc should return PDF_OK.
 *    2. The returned string must be the expected one, and NUL terminated
 */
START_TEST(pdf_text_get_pdfdocenc_001)
{
  extern const test_string_t pdf_doc_encoding_strings[];
  int i;

  i = 0;
  while (pdf_doc_encoding_strings[i].data)
    {
      pdf_error_t *error = NULL;
      pdf_text_t *text;
      pdf_char_t *data;
      pdf_size_t size;
      const pdf_char_t *expected_data;
      pdf_size_t expected_size;

      /* Set expected data and size */
      expected_data = (pdf_char_t *)pdf_doc_encoding_strings[i].data;
      expected_size = pdf_doc_encoding_strings[i].size;

      text = pdf_text_new_from_unicode ((pdf_char_t *) pdf_doc_encoding_strings[i].utf32be_data,
                                        (pdf_size_t) pdf_doc_encoding_strings[i].utf32be_size,
                                        PDF_TEXT_UTF32_BE,
                                        &error);
      fail_unless (text != NULL);
      fail_if (error != NULL);

      /* 1. The call to  pdf_text_get_pdfdocenc should return PDF_OK. */
      data = pdf_text_get_pdfdocenc (text, &error);
      fail_unless (data != NULL);
      fail_if (error != NULL);

      /* 2. The returned string must be the expected one, and NUL terminated */
      size = strlen (data);
      fail_unless (size == expected_size);
      fail_unless (memcmp (expected_data, data, size) == 0);

      pdf_text_destroy(text);
      pdf_dealloc (data);

      ++i;
    }
}
END_TEST

/*
 * Test: pdf_text_get_pdfdocenc_002
 * Description:
 *   Get the contents of a text object in PDF Doc Encoding. The contents of the
 *   text object cannot be transformed to PDF Doc Encoding without loss of
 *   information. A default character must be used for those characters that
 *   cannot be represented in PDF Doc Encoding.
 * Success conditions:
 *    1. The call to  pdf_text_get_pdfdocenc should return PDF_OK.
 *    2. The returned string must be the expected one, and NUL terminated
 */
START_TEST(pdf_text_get_pdfdocenc_002)
{
  pdf_error_t *error = NULL;
  pdf_text_t *text;
  const pdf_char_t *data = (pdf_char_t *) "\x00\x00\x6C\x34" "\x00\x00\x00\x7A" "\x00\x01\xD1\x1E";
  pdf_size_t size = 12;
  pdf_char_t *expected_data = (pdf_char_t *)"?z?";
  pdf_size_t expected_size = 3;
  pdf_char_t *output_data;

  text = pdf_text_new_from_unicode (data, size, PDF_TEXT_UTF32_BE, &error);
  fail_unless (text != NULL);
  fail_if (error != NULL);

  /* 1. The call to  pdf_text_get_pdfdocenc should return PDF_OK. */
  output_data = pdf_text_get_pdfdocenc (text, &error);
  fail_unless (output_data != NULL);
  fail_if (error != NULL);

  /* 2. The returned string must be the expected one, and NUL terminated */
  size = strlen (output_data);
  fail_unless (size == expected_size);
  fail_unless (memcmp (expected_data, output_data, size) == 0);

  pdf_text_destroy (text);
  pdf_dealloc (output_data);
}
END_TEST

/*
 * Test case creation function
 */
TCase *
test_pdf_text_get_pdfdocenc (void)
{
  TCase *tc = tcase_create ("pdf_text_get_pdfdocenc");

  tcase_add_test (tc, pdf_text_get_pdfdocenc_001);
  tcase_add_test (tc, pdf_text_get_pdfdocenc_002);
  tcase_add_checked_fixture (tc,
                             pdf_test_setup,
                             pdf_test_teardown);
  return tc;
}

/* End of pdf-text-get-pdfdocenc.c */
