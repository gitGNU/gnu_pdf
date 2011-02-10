/* -*- mode: C -*-
 *
 *       File:         pdf-stm-filter.h
 *       Date:         Thu Jun 12 22:05:06 2008
 *
 *       GNU PDF Library - Stream Filter
 *
 */

/* Copyright (C) 2008, 2009 Free Software Foundation, Inc. */

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

#ifndef PDF_STM_FILTER_H
#define PDF_STM_FILTER_H

#include <config.h>

#include <pdf-types.h>
#include <pdf-types-buffer.h>
#include <pdf-hash.h>
#include <pdf-stm-be.h>
#include <pdf-stm-f-null.h>
#include <pdf-stm-f-ahex.h>
#include <pdf-stm-f-rl.h>
#include <pdf-stm-f-v2.h>
#include <pdf-stm-f-aesv2.h>
#include <pdf-stm-f-md5.h>
#include <pdf-stm-f-lzw.h>
#include <pdf-stm-f-a85.h>

#if defined PDF_HAVE_LIBZ
#  include <pdf-stm-f-flate.h>
#endif /* PDF_HAVE_LIBZ */

#if defined PDF_HAVE_LIBJBIG2DEC
#  include <pdf-stm-f-jbig2.h>
#endif /* PDF_HAVE_LIBJBIG2DEC */

#if defined PDF_HAVE_LIBJPEG
#  include <pdf-stm-f-dct.h>
#endif

/* BEGIN PUBLIC */

/* Types of filters */
enum pdf_stm_filter_type_e
{
  PDF_STM_FILTER_NULL = 0,
  PDF_STM_FILTER_RL_ENC,
  PDF_STM_FILTER_RL_DEC,
#if defined PDF_HAVE_LIBZ
  PDF_STM_FILTER_FLATE_ENC,
  PDF_STM_FILTER_FLATE_DEC,
#endif /* PDF_HAVE_LIBZ */
#if defined PDF_HAVE_LIBJBIG2DEC
  PDF_STM_FILTER_JBIG2_DEC,
#endif /* PDF_HAVE_LIBJBIG2DEC */
#if defined PDF_HAVE_LIBJPEG
  PDF_STM_FILTER_DCT_DEC,
#endif /* PDF_HAVE_LIBJPEG */
  PDF_STM_FILTER_AHEX_ENC,
  PDF_STM_FILTER_AHEX_DEC,
  PDF_STM_FILTER_AESV2_ENC,
  PDF_STM_FILTER_AESV2_DEC,
  PDF_STM_FILTER_V2_ENC,
  PDF_STM_FILTER_V2_DEC,
  PDF_STM_FILTER_MD5_ENC,
  PDF_STM_FILTER_LZW_DEC,
  PDF_STM_FILTER_LZW_ENC,
  PDF_STM_FILTER_A85_DEC,
  PDF_STM_FILTER_A85_ENC
};

/* END PUBLIC */

enum pdf_stm_filter_mode_e
{
  PDF_STM_FILTER_MODE_READ,
  PDF_STM_FILTER_MODE_WRITE
};

typedef struct pdf_stm_filter_s *pdf_stm_filter_t;

/* Filter implementation */
struct pdf_stm_filter_impl_s
{
  pdf_status_t (*init_fn) (pdf_hash_t  *params,
                           void       **state);

  pdf_status_t (*apply_fn) (pdf_hash_t   *params,
                            void         *state,
                            pdf_buffer_t  in,
                            pdf_buffer_t  out,
                            pdf_bool_t    finish_p);
  pdf_status_t (*dealloc_state_fn) (void *state);
};

/* Filter data type */
struct pdf_stm_filter_s
{
  enum pdf_stm_filter_type_e type;

  struct pdf_stm_filter_s *next; /* Next filter in the chain, or
                                    NULL */
  pdf_stm_be_t backend;          /* Backend, or NULL */

  /* Input and output buffers */
  pdf_buffer_t in;
  pdf_buffer_t out;

  /* Filter-specific information */
  pdf_hash_t *params;
  void *state;

  /* Filter implementation */
  struct pdf_stm_filter_impl_s impl;

  /* Filter status */
  pdf_status_t status;
  pdf_bool_t really_finish_p;

  /* Operation mode */
  enum pdf_stm_filter_mode_e mode;
};

/*
 * Public API
 */

pdf_status_t pdf_stm_filter_new (enum pdf_stm_filter_type_e  type,
                                 pdf_hash_t                 *params,
                                 pdf_size_t                  buffer_size,
                                 enum pdf_stm_filter_mode_e  mode,
                                 pdf_stm_filter_t           *filter);
pdf_status_t pdf_stm_filter_destroy (pdf_stm_filter_t filter);
inline pdf_status_t pdf_stm_filter_set_next (pdf_stm_filter_t filter,
                                             pdf_stm_filter_t next_filter);
inline pdf_status_t pdf_stm_filter_set_be (pdf_stm_filter_t filter,
                                           pdf_stm_be_t     be);
inline pdf_status_t pdf_stm_filter_set_out (pdf_stm_filter_t filter,
                                            pdf_buffer_t     buffer);
pdf_stm_filter_t pdf_stm_filter_get_tail (pdf_stm_filter_t filter);
inline pdf_buffer_t pdf_stm_filter_get_in (pdf_stm_filter_t filter);

pdf_status_t pdf_stm_filter_apply (pdf_stm_filter_t filter,
                                   pdf_bool_t       finish_p);
pdf_status_t pdf_stm_filter_reset (pdf_stm_filter_t filter);

#endif /* ! PDF_STM_FILTER_H */

/* End of pdf-stm-filter.h */
