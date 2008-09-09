/* -*- mode: C -*- Time-stamp: "08/09/09 00:37:53 jemarch"
 *
 *       File:         pdf-error.c
 *       Date:         Sun Feb  24 20:22:05 2008
 *
 *       GNU PDF Library - Implementation for the Error module
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
#include <stdlib.h>
#include <stdarg.h>

#include <pdf-global.h>
#include <pdf-error.h>

extern char *program_name;

/* Update this list according to pdf_status_t. */
const char * pdf_error_stlist [] =
  {
          "error",
          "invalid or bad arguments",
          "insufficient memory",
          "end of file",
          "divison by zero",
          "no node found",
          "invalid range",
          "error in text encoding",
          "no matching found",
          "invalid text format",
          "not enough permissions",
          "invalid path name",
          "operation can't be performed now, maybe later",
          "no space left on device",
          "text module global context not initialized",
          "operation requires some entity be empty"
  };


void
pdf_perror (const pdf_status_t status, const char *str)
{ 
  pdf_error ((int) status, stderr, str);
}


void
pdf_error (const pdf_status_t status, FILE * fd, const char *format, ...)
{
  va_list args;
  int errnum;

  errnum = (int) status;

  if (fd == NULL)
    {
      fd = stderr;
    }
  
  fprintf (fd, "%s", program_name);

  if (format != NULL)
    {
      fprintf (fd, ": ");
      va_start (args, format);
      vfprintf (fd, format, args);
      va_end (args);
    }

  if (errnum >  0 && errnum < PDF_STATUS_ITEMS)
    fprintf (fd, ": %s", pdf_error_stlist[errnum-1]);
  
  fprintf (fd, ".\n");
  fflush (fd);

}

/* End of pdf-error.c */
