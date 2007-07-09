/* -*- mode: C -*- Time-stamp: "07/07/09 01:38:05 jemarch"
 *
 *       File:         pdf_stm_mem.c
 *       Author:       Jose E. Marchesi (jemarch@gnu.org)
 *       Date:         Mon Jul  9 01:13:41 2007
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

#include <config.h>
#include <string.h>
#include <malloc.h>
#include <xalloc.h>
#include <pdf_base.h>
#include <pdf_stm_mem.h>

static size_t pdf_stm_mem_readpeek (void *be_data, char *buf, size_t bytes, int peek);


int
pdf_stm_mem_init (void **be_data,
                  void *conf_data)
{
  pdf_stm_mem_data_t *data;
  pdf_stm_mem_conf_t conf;

  data = (pdf_stm_mem_data_t *) be_data;
  conf = (pdf_stm_mem_conf_t) conf_data;

  /* Create the private data storage */
  *data = (pdf_stm_mem_data_t) xmalloc (sizeof(struct pdf_stm_mem_data_s));

  /* Initialize private data */
  (*data)->size = conf->size;
  (*data)->data = (char *) xmalloc (conf->size);
  (*data)->current = 0;

  return PDF_OK;
}

inline int
pdf_stm_mem_write_p (void *be_data)
{
  return PDF_TRUE;
}

inline int
pdf_stm_mem_read_p (void *be_data)
{
  return PDF_TRUE;
}

inline int
pdf_stm_mem_seek_p (void *be_data)
{
  return PDF_TRUE;
}

inline int
pdf_stm_mem_size_p (void *be_data)
{
  return PDF_TRUE;
}

inline int
pdf_stm_mem_peek_p (void *be_data)
{
  return PDF_TRUE;
}

int
pdf_stm_mem_close (void **be_data)
{
  pdf_stm_mem_data_t *data;

  data = (pdf_stm_mem_data_t *) be_data;
  free ((*data)->data);
  free (*data);

  return PDF_OK;
}

pdf_stm_pos_t
pdf_stm_mem_size (void *be_data)
{
  pdf_stm_mem_data_t data;

  data = (pdf_stm_mem_data_t) be_data;
  return data->size;
}

int
pdf_stm_mem_seek (void *be_data,
                  pdf_stm_pos_t pos)
{
  pdf_stm_mem_data_t data;

  data = (pdf_stm_mem_data_t) be_data;

  if ((pos < 0) || (pos >= data->size))
    {
      return PDF_ERROR;
    }

  data->current = pos;
  return PDF_OK;
}

pdf_stm_pos_t
pdf_stm_mem_tell (void *be_data)
{
  pdf_stm_mem_data_t data;

  data = (pdf_stm_mem_data_t) be_data;
  return data->current;
}

size_t
pdf_stm_mem_read (void *be_data,
                  char *buf,
                  size_t bytes)
{
  return pdf_stm_mem_readpeek (be_data,
                               buf,
                               bytes,
                               PDF_FALSE);
}

size_t
pdf_stm_mem_write (void *be_data,
                   char *buf,
                   size_t bytes)
{
  pdf_stm_mem_data_t data;
  size_t written;

  data = (pdf_stm_mem_data_t) be_data;

  if ((data->current + bytes) >= data->size)
    {
      written = data->size - data->current;
    }
  else
    {
      written = bytes;
    }

  if (written > 0)
    {
      memcpy (data->data,
              buf,
              written);
      data->current = data->current + written;
    }
     
  return written;
}

inline size_t
pdf_stm_mem_peek (void *be_data,
                  char *buf,
                  size_t bytes)
{
  return pdf_stm_mem_readpeek (be_data,
                               buf,
                               bytes,
                               PDF_TRUE);
}

/* Private functions */

static size_t
pdf_stm_mem_readpeek (void *be_data,
                      char *buf,
                      size_t bytes,
                      int peek)
{
  pdf_stm_mem_data_t data;
  size_t readed;

  data = (pdf_stm_mem_data_t) be_data;

  if ((data->current + bytes) >= data->size)
    {
      readed = data->size - data->current;
    }
  else
    {
      readed = bytes;
    }

  if (readed > 0)
    {
      memcpy (buf,
              data->data + data->current,
              readed);

      if (!peek)
        {
          data->current = data->current + readed;
        }
    }

  return readed;
}

/* End of pdf_stm_mem.h */
