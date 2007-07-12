/* -*- mode: C -*- Time-stamp: "07/07/12 01:40:40 jemarch"
 *
 *       File:         pdf_stm_mem.h
 *       Author:       Jose E. Marchesi (jemarch@gnu.org)
 *       Date:         Sun Jul  8 21:45:09 2007
 *
 *       GNU PDF Library - Memory stream backend
 *
 */

/* Copyright (C) 2007 Jose E. Marchesi */

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

#ifndef PDF_STM_MEM_H
#define PDF_STM_MEM_H

#include <config.h>
#include <pdf_base.h>

/* Configuration structure */

struct pdf_stm_mem_conf_s
{
  pdf_stm_pos_t size;
  int init_p;
  int resize_p;
  pdf_char_t init_char;
};

typedef struct pdf_stm_mem_conf_s *pdf_stm_mem_conf_t;

/* Private data */

struct pdf_stm_mem_data_s
{
  pdf_char_t *data;
  pdf_stm_pos_t size;
  pdf_stm_pos_t current;
  int resize_p;
};

typedef struct pdf_stm_mem_data_s *pdf_stm_mem_data_t;

/* Backend API implementation */

int pdf_stm_mem_init (void **be_data, void *conf_data);

int pdf_stm_mem_write_p (void *be_data);
int pdf_stm_mem_read_p (void *be_data);
int pdf_stm_mem_seek_p (void *be_data);
int pdf_stm_mem_size_p (void *be_data);
int pdf_stm_mem_peek_p (void *be_data);

int pdf_stm_mem_close (void **be_data);

pdf_stm_pos_t pdf_stm_mem_size (void *be_data);
int pdf_stm_mem_seek (void *be_data, pdf_stm_pos_t pos);
pdf_stm_pos_t pdf_stm_mem_tell (void *be_data);

size_t pdf_stm_mem_read (void *be_data, pdf_char_t *buf, size_t bytes);
size_t pdf_stm_mem_write (void *be_data, pdf_char_t *buf, size_t bytes);
size_t pdf_stm_mem_peek (void *be_data, pdf_char_t *buf, size_t bytes);

#endif /* pdf_stm_mem.h */

/* End of pdf_stm_mem.h */
