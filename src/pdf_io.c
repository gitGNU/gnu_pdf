/* -*- mode: C -*- Time-stamp: "07/07/06 20:45:50 jemarch"
 *
 *       File:         pdf_io.c
 *       Author:       Jose E. Marchesi (jemarch@gnu.org)
 *       Date:         Fri Jul  6 18:43:15 2007
 *
 *       GNU PDF Library - Input/Output routines
 *
 */

#include <unistd.h>
#include <xalloc.h>
#include <pdf_io.h>

pdf_io_t 
pdf_io_open_file (char *filename)
{
  pdf_io_t new_io;

  if (!access (filename, R_OK))
    {
      return NULL;
    }

  new_io = (pdf_io_t) xmalloc (sizeof(struct pdf_io_s));
  new_io->type = PDF_IO_FILE;
  new_io->data.file.stream = 
    fopen (filename, "rb");

  return new_io;
}

int
pdf_io_close (pdf_io_t io)
{
  if (!fclose (io->data.file.stream))
    {
      return PDF_ERROR;
    }

  free (io);
  return PDF_OK;
}

static int
pdf_io_seek (pdf_io_t io,
             pdf_io_pos_t pos,
             int seek_mode)
{
  if (!fseek (io->data.file.stream,
              pos,
              seek_mode))
    {
      return PDF_ERROR;
    }
  
  return PDF_OK;
}

inline int
pdf_io_seek_beg (pdf_io_t io,
                 pdf_io_pos_t pos)
{
  if (pdf_io_seek (io, pos, SEEK_SET) == -1)
    {
      return PDF_ERROR;
    }
  
  return PDF_OK;
}

inline int
pdf_io_seek_cur (pdf_io_t io,
                 pdf_io_pos_t pos)
{
  if (pdf_io_seek (io, pos, SEEK_CUR) == -1)
    {
      return PDF_ERROR;
    }

  return PDF_OK;
}

inline int
pdf_io_seek_end (pdf_io_t io,
                 pdf_io_pos_t pos)
{
  if (pdf_io_seek (io, pos, SEEK_END) == -1)
    {
      return PDF_ERROR;
    }

  return PDF_OK;
}

inline pdf_io_pos_t
pdf_io_tell (pdf_io_t io)
{
  return ftell (io->data.file.stream);
}

inline size_t
pdf_io_read (pdf_io_t io,
             char *buf,
             size_t bytes)
{
  return fread (buf, 1, bytes, io->data.file.stream);
}

inline size_t
pdf_io_write (pdf_io_t io,
              char *buf,
              size_t bytes)
{
  return fwrite (buf, 1, bytes, io->data.file.stream);
}

/* End of pdf_io.h */
