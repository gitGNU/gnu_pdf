/* -*- mode: C -*-
 *
 *       File:         pdf-text-filter.c
 *       Date:         Fri Feb 25 23:58:56 2008
 *
 *       GNU PDF Library - Encoded Text Filters
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

#include <pdf-text-filter.h>
#include <pdf-text-context.h>
#include <pdf-text-ucd.h>

/* Change Case of text */
static pdf_bool_t
pdf_text_filter_change_case (pdf_text_t              *text,
                             enum unicode_case_type   new_case,
                             pdf_error_t            **error)
{
  pdf_size_t i;
  pdf_size_t n_words;
  pdf_size_t worst_length;
  pdf_size_t new_length;
  pdf_char_t *new_data;
  pdf_list_t *new_wb_list;

  const pdf_char_t *language;

  /* Generate original word boundaries list, if not already done */
  if (!pdf_text_generate_word_boundaries (text, error))
    return PDF_FALSE;

  /* Get text language ID. First, try to get it from the pdf_text_t element */
  language = pdf_text_get_language (text);
  /* If text element doesn't have a language ID, get it from the text context */
  if (strlen (language) == 0)
    language = pdf_text_context_get_host_language ();

  /* Worst length will be having 3 output UTF-32 characters per each input
   *  UTF-32 character. First of all, allocate memory for the worst length */
  worst_length = text->size * UCD_SC_MAX_EXPAND;
  new_data = (pdf_char_t *) pdf_alloc (worst_length);
  if (!new_data)
    {
      pdf_set_error (error,
                     PDF_EDOMAIN_BASE_TEXT,
                     PDF_ENOMEM,
                     "cannot change case of text: "
                     "couldn't allocate %lu bytes",
                     (unsigned long)worst_length);
      return PDF_FALSE;
    }

  /* Create new empty word boundaries list */
  new_wb_list = pdf_text_create_word_boundaries_list (error);
  if (new_wb_list == NULL)
    {
      pdf_dealloc (new_data);
      return PDF_FALSE;
    }

  /* Walk list of words, uppercasing all of them */
  n_words = pdf_list_size (text->word_boundaries);
  new_length = 0;
  for (i = 0; i < n_words; ++i)
    {
      struct pdf_text_wb_s *p_new_word;
      const struct pdf_text_wb_s *p_word;
      pdf_size_t new_word_length = 0;

      /* Allocate new word */
      p_new_word = (struct pdf_text_wb_s *) pdf_alloc (sizeof (struct pdf_text_wb_s));
      if (!p_new_word)
        {
          pdf_set_error (error,
                         PDF_EDOMAIN_BASE_TEXT,
                         PDF_ENOMEM,
                         "cannot change case of text: "
                         "couldn't allocate %lu bytes",
                         (unsigned long)sizeof (struct pdf_text_wb_s));
          return PDF_FALSE;
        }

      /* Get word to process from list of words */
      p_word = pdf_list_get_at (text->word_boundaries, i, error);
      if (!p_word)
        {
          pdf_dealloc (new_data);
          pdf_list_destroy (new_wb_list);
          return PDF_FALSE;
        }

      /* Apply the case algorithm to the full word */
      if (!pdf_text_ucd_word_change_case (&new_data[new_length],
                                          &new_word_length,
                                          new_case,
                                          p_word->word_start,
                                          p_word->word_size,
                                          language,
                                          error))
        {
          pdf_list_destroy (new_wb_list);
          pdf_dealloc (new_data);
          pdf_dealloc (p_new_word);
          return PDF_FALSE;
        }

      /* Create new word info */
      p_new_word->word_start = &new_data[new_length];
      p_new_word->word_size = new_word_length;
      p_new_word->word_stop = &new_data[new_length + new_word_length -4];

      /* Add word to new list */
      if (!pdf_list_add_last (new_wb_list, p_new_word, error))
        return PDF_FALSE;

      /* Update new length */
      new_length += new_word_length;
    }

  /* Finally, reset the buffer length to its correct size */
  if (new_length != worst_length)
    {
      new_data = (pdf_char_t *) pdf_realloc (new_data, new_length);
      if (!new_data)
        {
          pdf_set_error (error,
                         PDF_EDOMAIN_BASE_TEXT,
                         PDF_ENOMEM,
                         "cannot change case of text: "
                         "couldn't allocate %lu bytes",
                         (unsigned long)new_length);
          pdf_text_destroy_word_boundaries_list (&new_wb_list);
          return PDF_FALSE;
        }
    }

  /* Replace contents (data and word boundary list) */
  pdf_dealloc (text->data);
  text->data = new_data;
  text->size = new_length;
  pdf_text_destroy_word_boundaries_list (&(text->word_boundaries));
  text->word_boundaries = new_wb_list;

  return PDF_TRUE;
}

/* Make all text Upper Case */
pdf_bool_t
pdf_text_filter_upper_case (pdf_text_t   *text,
                            pdf_error_t **error)
{
  return pdf_text_filter_change_case (text,
                                      UNICODE_CASE_INFO_UPPER_CASE,
                                      error);
}

/* Make all text Lower Case */
pdf_bool_t
pdf_text_filter_lower_case (pdf_text_t   *text,
                            pdf_error_t **error)
{
  return pdf_text_filter_change_case (text,
                                      UNICODE_CASE_INFO_LOWER_CASE,
                                      error);
}

/* Make all text Title Case */
pdf_bool_t
pdf_text_filter_title_case (pdf_text_t   *text,
                            pdf_error_t **error)
{
  return pdf_text_filter_change_case (text,
                                      UNICODE_CASE_INFO_TITLE_CASE,
                                      error);
}

/* Remove all single ampersands, and turn ' && ' into ' & ' */
pdf_bool_t
pdf_text_filter_remove_amp (pdf_text_t   *text,
                            pdf_error_t **error)
{
  return (((!pdf_text_replace_ascii (text, " ", " & ", error)) ||
           (!pdf_text_replace_ascii (text, " & ", " && ", error))) ?
          PDF_FALSE : PDF_TRUE);
}

/* Convert all ASCII code points to their Full-Width variants. These Full-Width
 *   variants are located in the FF00-FF60 range as follows:
 * - The range U+FF01-U+FF5E contains the full width variants of the ASCII
 *   characters in the range U+0021-U+007E.
 * - The range U+FF5F-U+FF60 contains the full width variants of double
 *   parentheses in the range U+2985-U+2986
 */
pdf_bool_t
pdf_text_filter_normalize_full_width_ascii (pdf_text_t   *text,
                                            pdf_error_t **error)
{
  pdf_size_t i;
  const pdf_u32_t delta = 0xFF01 - 0x0021;
  const pdf_u32_t delta2 = 0xFF5F - 0x2985;

  for (i = 0; i < text->size; i += 4)
    {
      pdf_u32_t unicode_point;

      /* Get unicode point in UTF-32HE */
      memcpy (&unicode_point, &(text->data[i]), 4);

      /* Check ranges */
      if ((unicode_point <= 0x007E) &&
          (unicode_point >= 0x0021))
        {
          unicode_point += delta;
          memcpy (&(text->data[i]), &unicode_point, 4);
        }
      else if ((unicode_point >= 0x2985) &&
               (unicode_point <= 0x2986))
        {
          unicode_point += delta2;
          memcpy(&(text->data[i]), &unicode_point, 4);
        }
    }

  return PDF_TRUE;
}

/* Substitute line endings with a given UTF-8 pattern. */
static pdf_bool_t
pdf_text_substitute_line_ending (pdf_text_t            *text,
                                 const pdf_text_eol_t   new_eol,
                                 pdf_error_t          **error)
{
  pdf_bool_t ret;
  int i;
  pdf_text_t *new_text_pattern;
  pdf_text_t **eols;

  /* Allocate memory for pdf_text_t old eols */
  eols = (pdf_text_t **) pdf_alloc (PDF_TEXT_EOLMAX * sizeof (pdf_text_t *));
  if (!eols)
    {
      pdf_set_error (error,
                     PDF_EDOMAIN_BASE_TEXT,
                     PDF_ENOMEM,
                     "cannot substitute line endings: "
                     "couldn't allocate %lu bytes",
                     (unsigned long)(PDF_TEXT_EOLMAX * sizeof (pdf_text_t)));
      return PDF_FALSE;
    }

  /* Create text new pattern */
  new_text_pattern = pdf_text_new_from_unicode (new_eol->sequence,
                                                strlen (new_eol->sequence),
                                                PDF_TEXT_UTF8,
                                                error);
  if (!new_text_pattern)
    {
      pdf_dealloc (eols);
      return PDF_FALSE;
    }

  /* For each possible EOL type, create a pdf_text_t */
  for (i = PDF_TEXT_EOL_WINDOWS; i < PDF_TEXT_EOLMAX; ++i)
    {
      pdf_text_eol_t requested_eol;

      /* Get Host EOL */
      requested_eol = pdf_text_context_get_host_eol ((enum pdf_text_eol_types)i);

      /* Create text old pattern */
      eols[i] = pdf_text_new_from_unicode (requested_eol->sequence,
                                           strlen (requested_eol->sequence),
                                           PDF_TEXT_UTF8,
                                           error);
      if (!eols[i])
        {
          int j;

          pdf_text_destroy (new_text_pattern);
          for (j = 0; j < i; ++j)
            pdf_text_destroy (eols[j]);
          pdf_dealloc (eols);
          return PDF_FALSE;
        }
    }

  /* Perform the replacement */
  ret = pdf_text_replace_multiple (text,
                                   new_text_pattern,
                                   eols,
                                   PDF_TEXT_EOLMAX,
                                   error);

  /* Destroy used patterns */
  for (i = PDF_TEXT_EOL_WINDOWS; i < PDF_TEXT_EOLMAX; i++)
    pdf_text_destroy (eols[i]);
  pdf_dealloc (eols);
  pdf_text_destroy (new_text_pattern);

  return ret;
}

/* Normalize all EOL sequences to the default host EOL */
pdf_bool_t
pdf_text_filter_normalize_line_endings (pdf_text_t   *text,
                                        pdf_error_t **error)
{
  pdf_text_eol_t host_eol;

  /* Get this host EOL */
  host_eol = pdf_text_context_get_host_eol (PDF_TEXT_EOL_HOST);
  /* Finally, substitute line endings */
  return pdf_text_substitute_line_ending (text,
                                          host_eol,
                                          error);
}

/* Replace EOL sequences with white spaces */
pdf_bool_t
pdf_text_filter_remove_line_endings (pdf_text_t   *text,
                                     pdf_error_t **error)
{
  const struct pdf_text_eol_s empty_eol =  { { 0x00, 0x00, 0x00 } };

  /* Substitute line endings */
  return pdf_text_substitute_line_ending (text,
                                          (pdf_text_eol_t)&empty_eol,
                                          error);
}

/* End of pdf-text-filter.c */
