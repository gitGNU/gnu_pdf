/* -*- mode: C -*- Time-stamp: "08/02/10 23:22:44 jemarch"
 *
 *       File:         pdf_stm_f_ahex.h
 *       Date:         Fri Jul 13 17:04:29 2007
 *
 *       GNU PDF Library - ASCIIHex encoder/decoder
 *
 */

/* Copyright (C) 2007 Free Software Foundation, Inc. */

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

#ifndef PDF_STM_F_AHEX_H
#define PDF_STM_F_AHEX_H

#include <config.h>
#include <pdf-base.h>

/* Configuration data */

/* BEGIN PUBLIC */

enum pdf_stm_f_ahex_mode_t
{
  PDF_STM_F_AHEX_MODE_ENCODE,
  PDF_STM_F_AHEX_MODE_DECODE
};


/* END PUBLIC */

struct pdf_stm_f_ahex_conf_s
{
  int mode;
};

typedef struct pdf_stm_f_ahex_conf_s *pdf_stm_f_ahex_conf_t;

/* Private data */

struct pdf_stm_f_ahex_data_s
{
  int mode;
};

typedef struct pdf_stm_f_ahex_data_s *pdf_stm_f_ahex_data_t;

/* Filter API implementation */

int pdf_stm_f_ahex_init (void **filter_data, void *conf_data);
int pdf_stm_f_ahex_apply (void *filter_data,
                          pdf_char_t *in, pdf_stm_pos_t in_size,
                          pdf_char_t **out, pdf_stm_pos_t *out_size);
int pdf_stm_f_ahex_dealloc (void **filter_data);

#endif /* pdf_stm_f_ahex.h */

/* End of pdf_stm_f_ahex.h */
