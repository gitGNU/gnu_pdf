/* -*- mode: C -*- Time-stamp: "08/09/20 20:25:04 jemarch"
 *
 *       File:         pdf-stm-f-null.h
 *       Date:         Mon Jul  9 21:59:50 2007
 *
 *       GNU PDF Library - NULL stream filter
 *
 */

/* Copyright (C) 2007, 2008 Free Software Foundation, Inc. */

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

#ifndef PDF_STM_F_NULL_H
#define PDF_STM_F_NULL_H

#include <config.h>
#include <pdf-types.h>
#include <pdf-hash.h>
#include <pdf-stm-buffer.h>

/* Filter implementation API */

pdf_status_t pdf_stm_f_null_init (pdf_hash_t params,
                                  pdf_hash_t state);

pdf_status_t pdf_stm_f_null_apply (pdf_hash_t params,
                                   pdf_hash_t state,
                                   pdf_stm_buffer_t in,
                                   pdf_stm_buffer_t out);
pdf_status_t pdf_stm_f_null_finish (pdf_hash_t params,
                                    pdf_hash_t state,
                                    pdf_stm_buffer_t out);

#endif /* !PDF_STM_F_NULL_H */

/* End of pdf_stm_f_null.h */
