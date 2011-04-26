/* -*- mode: C -*-
 *
 *       File:         pdf-text-test-common.h
 *       Date:         Mon Mar 10 18:30:01 2008
 *
 *       GNU PDF Library - Commont utilities to test the Text Module
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

#ifndef _PDF_TEXT_TEST_COMMON_H
#define _PDF_TEXT_TEST_COMMON_H

#include <config.h>
#include <stdio.h>
#include <pdf.h>
#include <pdf-test-common.h>

/* Basic structure to store the test strings */
typedef struct test_string_s {
  pdf_char_t *data;
  pdf_size_t  size;
  pdf_char_t *utf32be_data;
  pdf_size_t  utf32be_size;
} test_string_t;

/* Change UTF-32BE string to UTF-32LE and viceversa */
pdf_char_t *pdf_text_test_change_utf32_endianness (const pdf_char_t *str_in,
                                                   pdf_size_t        size);

/* Change UTF-16BE string to UTF-16LE and viceversa */
pdf_char_t *pdf_text_test_change_utf16_endianness (const pdf_char_t *str_in,
                                                   pdf_size_t        size);

/* Get BOM for a given unicode encoding */
const pdf_char_t *pdf_text_test_get_bom (enum pdf_text_unicode_encoding_e  enc,
                                         int                              *bom_size);

#ifdef TEXT_MODULE_ADDITIONAL_TEST_TRACES
# warning Additional test traces enabled in text module unit tests
# define PRINT_CONTENTS(function,index,text,expected,expected_size,remaining_size) { \
    pdf_char_t *expected_hex;                                           \
    pdf_char_t *actual;                                                 \
    pdf_size_t actual_size;                                             \
                                                                        \
    actual = pdf_text_get_unicode (text,                                \
                                   PDF_TEXT_UTF32_HE,                   \
                                   PDF_TEXT_UNICODE_NO_OPTION,          \
                                   &actual_size,                        \
                                   NULL);                               \
    printf ("%s:%d\n", function, index);                                \
    if (!actual)                                                        \
      printf ("  Error while retrieving internal data\n");              \
    else                                                                \
      {                                                                 \
        pdf_char_t *actual_hex;                                         \
                                                                        \
        actual_hex = pdf_test_get_hex (actual, actual_size, ':');       \
        printf ("  Internal> '%s'\n", actual_hex);                      \
        pdf_dealloc (actual_hex);                                       \
      }                                                                 \
    expected_hex = pdf_test_get_hex (expected, expected_size,':');      \
    printf ("  Expected> '%s'\n", expected_hex);                        \
    printf ("  Lang> '%s'\n", pdf_text_get_language (text));            \
    printf ("  Country> '%s'\n", pdf_text_get_country (text));          \
    printf ("  Remaining length> '%llu'\n", (unsigned long long)remaining_size); \
    pdf_dealloc (expected_hex);                                         \
  }
#else
# define PRINT_CONTENTS(...)
#endif /* TEXT_MODULE_ADDITIONAL_TEST_TRACES */

#endif /* _PDF_TEXT_TEST_COMMON_H */

/* End of pdf-text-test-common.h */
