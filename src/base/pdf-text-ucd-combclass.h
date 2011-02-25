/* -*- mode: C -*-
 *
 *       File:         pdf-text-ucd-combclass.h
 *       Date:         Sun Mar 09 12:59:01 2008
 *
 *       GNU PDF Library - Unicode Character Database - Combining Class
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

#ifndef _PDF_TEXT_UCD_COMBCLASS_H
#define _PDF_TEXT_UCD_COMBCLASS_H

#include <config.h>

#include <pdf-types.h>

/* Get Combining Class of a given UTF-32HE unicode point */
pdf_u8_t pdf_text_ucd_get_combining_class (const pdf_u32_t character);

#endif

/* End of pdf-text-ucd-combclass.h */
