/* -*- mode: C -*- Time-stamp: "08/09/09 00:59:14 jemarch"
 *
 *       File:         pdf-fsys-disk.c
 *       Date:         Thu May 22 18:27:35 2008
 *
 *       GNU PDF Library - Disk Filesystem Implementation
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

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <dirent.h>
#include <unistd.h>

#include <pdf-types.h>
#include <pdf-error.h>
#include <pdf-fsys-disk.h>

#ifndef PDF_HOST_WIN32
 #ifndef PDF_HOST_BSD
  #include <sys/statfs.h>
 #else
  #include <sys/param.h>
  #include <sys/mount.h>
 #endif
#else
#include <windows.h>
#endif /* !PDF_HOST_WIN32 */

/* Private function declarations */

static pdf_status_t
pdf_fsys_disk_get_host_path (pdf_text_t path,
                             pdf_char_t **host_path,
                             pdf_u32_t *host_path_size);

static pdf_status_t
pdf_fsys_disk_build_mode_string (const enum pdf_fsys_file_mode_e mode,
                                 pdf_char_t *mode_str);
static pdf_bool_t
pdf_fsys_disk_win32_device_p (pdf_text_t path);

/*
 * Filesystem Interface Implementation
 */

pdf_size_t
pdf_fsys_disk_get_free_space (pdf_text_t path_name)
{
  pdf_char_t *host_path;
  pdf_u32_t host_path_size;
#if defined PDF_HOST_WIN32
  pdf_char_t drive_letter[4];
  DWORD free_clusters;
  DWORD sectors_per_cluster;
  DWORD bytes_per_sector;
  DWORD dummy;
#else
  struct statfs fs_stats;
#endif /* !PDF_HOST_WIN32 */
  pdf_size_t result;

  /* Get the pathname in the host encoding */
  if (pdf_fsys_disk_get_host_path (path_name,
                                   &host_path,
                                   &host_path_size) != PDF_OK)
    {
      return 0;
    }

#if defined PDF_HOST_WIN32
  /* Get the drive letter of the specified path */

#if 0
  /* GetVolumePathName is declared but seems to not be implemented in
     mingw -jemarch */

  /* Microplof dixit: "If you specify a relative directory or file
     name without a volume qualifier, GetVolumePathName returns the
     drive letter of the boot volume." */
  if (GetVolumePathName ((char *) host_path,
                         &drive_letter,
                         host_path_size) == 0)
    {
      /* Cleanup */
      pdf_dealloc (host_path);
      /* Report error */
      return 0;
    }
#else
  /* So lets do the piggy hack that wont work with relative paths nor
     with multi-byte encodings */
  memcpy (drive_letter, host_path, 3);
  host_path[3] = 0;
#endif /* !0 */

  /* Get the information from the filesystem */
  if (! GetDiskFreeSpace ((char *) drive_letter,
                          &sectors_per_cluster, 
                          &bytes_per_sector,
                          &free_clusters, &dummy))
    {
      /* Cleanup */
      pdf_dealloc (host_path);
      /* Report error */
      return 0;
    }

  result = (bytes_per_sector * sectors_per_cluster
            * free_clusters); 
 
#else /* Non-windows plattform */
  if (statfs ((const char *) host_path, &fs_stats) != 0)
    {
      /* Cleanup */
      pdf_dealloc (host_path);

      /* Return an error */
      return 0;
    }

  result = (fs_stats.f_bfree * fs_stats.f_bsize);

#endif /* !PDF_HOST_WIN32 */

  /* Return the free space in octects */
  return result;
}


pdf_status_t
pdf_fsys_disk_open (const pdf_text_t path_name,
                    const enum pdf_fsys_file_mode_e mode,
                    pdf_fsys_file_t file)
{
  pdf_fsys_disk_file_t file_data;
  pdf_char_t mode_str[4];
  pdf_status_t ret_status;

  /* Allocate private data storage for the file */
  file_data = 
    (pdf_fsys_disk_file_t) pdf_alloc (sizeof(struct pdf_fsys_disk_file_s));
  file->data = (void *) file_data;
  
  /* Make and store a copy of the unicode file path and get the host
     encoded path */
  file_data->unicode_path = pdf_text_dup (path_name);
  if (pdf_fsys_disk_get_host_path (file_data->unicode_path,
                                   &file_data->host_path,
                                   &file_data->host_path_size))
    {
      /* Cleanup and report error */
      pdf_text_destroy (file_data->unicode_path);
      pdf_dealloc (file_data);

      return PDF_ERROR;
    }

  /* Build the mode string for fopen */
  pdf_fsys_disk_build_mode_string (mode, mode_str);

  /* Open the file */
  file_data->file_descriptor =
    fopen ((char *) file_data->host_path,
           (char *) mode_str);

  if (file_data->file_descriptor == NULL)
    {
      /* Cleanup and report error */
      pdf_text_destroy (file_data->unicode_path);
      pdf_dealloc (file_data->host_path);
      pdf_dealloc (file_data);
      pdf_dealloc (mode_str);

      switch (errno)
        {
#ifndef PDF_HOST_WIN32
          /* On Windows platforms (excluding Cygwin), fopen does not
             set errno upon failure. */

        case EACCES:
        case EPERM:
        case EROFS:
        case ETXTBSY:
          {
            /* Not enough permissions */
            ret_status = PDF_EBADPERMS;
            break;
          }
        case EISDIR:
        case ENAMETOOLONG:
        case ENOTDIR:
          {
            /* Invalid path name */
            ret_status = PDF_EBADNAME;
            break;
          }
        case ENOMEM:
          {
            /* Not enough memory */
            ret_status = PDF_ENOMEM;
            break;
          }
#endif /* !PDF_HOST_WIN32 */
        default:
          {
            /* Other error */
            ret_status = PDF_ERROR;
            break;
          }
        }

      file_data->file_mode = mode;

      return ret_status;
    }

  /* Set the filesystem for the file */
  file->fs = NULL; /* This is the default filesystem */

  /* All was ok */
  return PDF_OK;
}

pdf_status_t
pdf_fsys_disk_create_folder (const pdf_text_t path_name)
{
  pdf_char_t *host_path;
  pdf_u32_t host_path_size;
  mode_t dir_mode;

  /* Get a host-encoded version of the path name */
  if (pdf_fsys_disk_get_host_path (path_name, 
                                   &host_path, 
                                   &host_path_size) != PDF_OK)
    {
      return PDF_ERROR;
    }

  /* Set the permissions of the new directory:
     
     rwxr_xr_x

  */
  dir_mode = S_IRUSR | S_IWUSR | S_IXUSR
    | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

  /* Open the file */
  if (mkdir ((const char *) host_path, dir_mode) != 0)
    {
      switch (errno)
        {
          /* File name syntax errors */
        case ENAMETOOLONG:
        case ENOENT:
        case ENOTDIR:
#ifndef PDF_HOST_WIN32
        case ELOOP:
#endif /* !PDF_HOST_WIN32 */
          {
            return PDF_EBADNAME;
            break;
          }
          /* Other specific errors */
        case EACCES:
          {
            return PDF_EBADPERMS;
            break;
          }
        case EEXIST:
        case EMLINK:
        case ENOSPC:
        case EROFS:
          /* Any other error */
        default:
          {
            return PDF_ERROR;
            break;
          }
        }
    }

  /* The directory was successfully created */
  return PDF_OK;
}

pdf_status_t
pdf_fsys_disk_get_folder_contents (const pdf_text_t path_name,
                                   pdf_list_t item_list)
{
  DIR *dir_stream;
  struct dirent *dir_entry;
  pdf_char_t *host_path;
  pdf_u32_t host_path_size;
  pdf_text_t entry_text;
  pdf_u32_t name_length;
  

  /* Get the pathname in the host encoding */
  if (pdf_fsys_disk_get_host_path (path_name,
                                   &host_path,
                                   &host_path_size) != PDF_OK)
    {
      return PDF_ERROR;
    }

  /* Open the directory stream */
  dir_stream = opendir ((const char*) host_path);
  if (dir_stream == NULL)
    {
      switch (errno)
        {
          /* File name syntax errors */
        case ENAMETOOLONG:
        case ENOENT:
        case ENOTDIR:
#ifndef PDF_HOST_WIN32
        case ELOOP:
#endif /* !PDF_HOST_WIN32 */
          {
            return PDF_EBADNAME;
            break;
          }
          /* Other specific errors */
        case EACCES:
          {
            return PDF_EBADPERMS;
            break;
          }
        case EMFILE:
        case ENFILE:
        case ENOMEM:
          /* Any other error */
        default:
          {
            return PDF_ERROR;
            break;
          }
        }
    }

  /* Scan directory contents */
  while ((dir_entry = readdir (dir_stream)) != NULL)
    {
      /* Note that dir_entry is statically allocated and can be
         rewritten by a subsequent call. Also, there is not need to
         free that structure */

      /* Get the length of the entry name */
#if defined PDF_HOST_WIN32
      /* In mingw dir_entry->d_namlen is an array of FILENAME_MAX
         octects long. The dir_entry->d_namlen contain the length of
         the name stored in d_name */
      name_length = dir_entry->d_namlen;
#else
      /* In POSIX systems dir_entry->d_name is a NULL-terminated
         string */
      name_length = strlen (dir_entry->d_name);
#endif /* PDF_HOST_WIN32 */

      /* Create the text object containing the entry name */
      if ((pdf_text_new_from_host ((pdf_char_t *) dir_entry->d_name,
                                   name_length,
                                   pdf_text_get_host_encoding(),
                                   &entry_text) != PDF_OK) ||
          (pdf_list_add_last (item_list, 
                              (void *) entry_text, NULL) != PDF_OK))
        {
          /* Cleanup and report an error */
          closedir (dir_stream);
          return PDF_ERROR;
        }
    }

  /* All was ok, so return with a beautiful OK */
  return PDF_OK;
}


pdf_status_t
pdf_fsys_disk_get_parent (const pdf_text_t path_name,
                          pdf_text_t parent_path)
{
  /* FIXME: Implement me :D */
  return PDF_OK;
}

pdf_status_t
pdf_fsys_disk_remove_folder (const pdf_text_t path_name)
{
  pdf_char_t *host_path;
  pdf_u32_t host_path_size;

  /* Get the pathname in the host encoding */
  if (pdf_fsys_disk_get_host_path (path_name,
                                   &host_path,
                                   &host_path_size) != PDF_OK)
    {
      return PDF_ERROR;
    }

  /* Try to remove the directory */
#ifndef ENOTEMPTY
#define ENOTEMPTY EEXIST
#endif /* !ENOTEMPTY */
  if (rmdir ((const char *) host_path) != 0)
    {
      /* Cleanup */
      pdf_dealloc (host_path);

      /* Manage the error condition */
      switch (errno)
        {
          /* File name syntax errors */
        case ENAMETOOLONG:
        case ENOENT:
        case ENOTDIR:
#ifndef PDF_HOST_WIN32
        case ELOOP:
#endif /* !PDF_HOST_WIN32 */
          {
            return PDF_EBADNAME;
            break;
          }
          /* Specific error conditions */
        case EACCES:
          {
            return PDF_EBADPERMS;
            break;
          }
        case ENOTEMPTY:
          {
            return PDF_ENOTEMPTY;
            break;
          }
          /* Any other error condition */
        default:
          {
            return PDF_ERROR;
            break;
          }
        }
    }

  /* Cleanup and return a success status */
  pdf_dealloc (host_path);
  return PDF_OK;
}

pdf_status_t
pdf_fsys_disk_get_item_props (pdf_text_t path_name,
                              struct pdf_fsys_item_props_s *item_props)
{
  pdf_char_t* ascii_path;
  pdf_u32_t ascii_path_len;
  pdf_status_t ret_code;
  unsigned int access_mode;
#ifndef PDF_HOST_WIN32
  struct stat file_info;
#else
  WIN32_FIND_DATA file_info;
  HANDLE file_handle;
#endif

  if (path_name == NULL)
    {
      return PDF_EBADDATA;
    }

  ret_code = pdf_fsys_disk_get_host_path (path_name, &ascii_path, &ascii_path_len);

  if (ret_code != PDF_OK)
    {
      goto set_error;
    }

  if ((access_mode = access((char*)ascii_path, R_OK)))
    {
      goto set_error;
    }
  else
    {
      item_props->is_readable = PDF_TRUE;
    }

  if ((access_mode = access((char*)ascii_path, W_OK)))
    {
      goto set_error;
    }
  else
    {
      item_props->is_writable = PDF_TRUE;
    }

#ifndef PDF_HOST_WIN32
  if (stat((char*)ascii_path, &file_info))
    {
      goto set_error;
    }
  item_props->file_size_high = 0;
  item_props->file_size_low = file_info.st_size;
#else /* !PDF_HOST_WIN32 */
  file_handle = FindFirstFile((char *) ascii_path, &file_info);
  if (file_handle == INVALID_HANDLE_VALUE)
    {
      goto set_error;
    }
  else
    {
      if(CloseHandle(file_handle) == FALSE)
        {
          goto set_error;
        }
    }
  item_props->file_size_high = file_info.nFileSizeHigh;
  item_props->file_size_low = file_info.nFileSizeLow;
#endif

  if (ascii_path)
    {
      pdf_dealloc (ascii_path);
    }
  return PDF_OK;

 set_error:
  switch(errno)
    {
      /* File name syntax errors */
    case ENAMETOOLONG:
    case ENOENT:
    case ENOTDIR:
#ifndef PDF_HOST_WIN32
    case ELOOP:
#endif /* !PDF_HOST_WIN32 */
      {
        ret_code = PDF_EBADNAME;
        break;
      }
      /* Specific error conditions */
    case EACCES:
      {
        ret_code = PDF_EBADPERMS;
        break;
      }
    case ENOTEMPTY:
      {
        ret_code = PDF_ENOTEMPTY;
        break;
      }
      /* Any other error condition */
    default:
      {
        ret_code = PDF_ERROR;
        break;
      }
    }

  if (ascii_path)
    {
      pdf_dealloc (ascii_path);
    }
  return ret_code;
}

pdf_bool_t
pdf_fsys_disk_item_p (pdf_text_t path_name)
{
  struct pdf_fsys_item_props_s item_props;
  pdf_status_t status;


  if (pdf_fsys_disk_win32_device_p(path_name) == PDF_TRUE)
    {
      return PDF_TRUE;
    }

  status = pdf_fsys_disk_get_item_props(path_name, &item_props);

  if (status != PDF_OK)
    {
      return PDF_FALSE;
    }

  return PDF_TRUE;
}

pdf_bool_t 
pdf_fsys_disk_item_readable_p (pdf_text_t path_name)
{
  struct pdf_fsys_item_props_s item_props;
  pdf_status_t status;

  status = pdf_fsys_disk_get_item_props(path_name, &item_props);

  if (status != PDF_OK)
    {
      return PDF_FALSE;
    }

  return item_props.is_readable;
}

pdf_bool_t
pdf_fsys_disk_item_writable_p (pdf_text_t path_name)
{
  struct pdf_fsys_item_props_s item_props;
  pdf_status_t status;

  status = pdf_fsys_disk_get_item_props(path_name, &item_props);

  if (status != PDF_OK)
    {
      return PDF_FALSE;
    }

  return item_props.is_writable;
}

pdf_text_t
pdf_fsys_disk_get_temp_path_name (void)
{
  /* FIXME: Please implement me :D */
  return NULL;
}

/*
 * File Interface Implementation
 */

enum pdf_fsys_file_mode_e
pdf_fsys_disk_file_get_mode (pdf_fsys_file_t file)
{
  return ((pdf_fsys_disk_file_t)file->data)->file_mode;
}

pdf_text_t 
pdf_fsys_disk_file_get_url (pdf_fsys_file_t file)
{
  /* FIXME: Please implement me :D */
  return NULL;
}

pdf_status_t
pdf_fsys_disk_file_set_mode (pdf_fsys_file_t file,
                             enum pdf_fsys_file_mode_e new_mode)
{
  pdf_char_t mode_str[4];
  pdf_status_t result_status;
  pdf_fsys_disk_file_t work_file = (pdf_fsys_disk_file_t)file->data;

  pdf_fsys_disk_build_mode_string(new_mode, mode_str);

  work_file->file_descriptor = freopen((char *) work_file->host_path,
                                       (char *) mode_str,
                                       work_file->file_descriptor);

  work_file->file_mode = new_mode;

  if (work_file->file_descriptor == NULL)
    {
      switch (errno)
        {
#ifndef PDF_HOST_WIN32
          /* On Windows platforms (excluding Cygwin), freopen does not
             set errno upon failure. */

        case EBADF:
        case EFAULT:
        case EFBIG:
        case EINVAL:
          {
            result_status = PDF_EBADDATA;
            break;
          }
        case EAGAIN:
        case ENOSPC:
#endif /* !PDF_HOST_WIN32 */
        default:
          {
            /* Other error conditions */
            result_status = PDF_ERROR;
            break;
          }
        }
    }
  else
    {
      result_status = PDF_OK;
    }

  return result_status;
}

pdf_bool_t
pdf_fsys_disk_file_same_p (pdf_fsys_file_t file,
                           pdf_text_t path)
{
  pdf_status_t stat;
  pdf_i32_t ret;
  pdf_bool_t case_sensitive;
  pdf_fsys_disk_file_t work_file = (pdf_fsys_disk_file_t)file->data;

  /* Unix-like systems have case sensitive paths, Windows doesn't. */
#ifndef PDF_HOST_WIN32
  case_sensitive = PDF_TRUE;
#else
  case_sensitive = PDF_FALSE;
#endif

  ret = pdf_text_cmp(work_file->unicode_path, path, case_sensitive, &stat);

  if (ret == 0 && stat == PDF_OK)
    {
      return PDF_TRUE;
    }

  return PDF_FALSE;
}

pdf_status_t
pdf_fsys_disk_file_get_pos (pdf_fsys_file_t file,
                            pdf_size_t *pos)
{
  pdf_status_t result_status;
  pdf_fsys_disk_file_t work_file = (pdf_fsys_disk_file_t)file->data;

  if (fgetpos(work_file->file_descriptor, (fpos_t*)pos) == 0)
    {
      result_status = PDF_OK;
    }
  else
    {
      switch (errno)
        {
        case EINVAL:
        case ESPIPE:
        case EOVERFLOW:
        case EBADF:
          {
            result_status = PDF_EBADDATA;
            break;
          }
        default:
          {
            /* Other error conditions */
            result_status = PDF_ERROR;
            break;
          }
        }
    }

  return result_status;
}

pdf_status_t
pdf_fsys_disk_file_set_pos (pdf_fsys_file_t file,
                            pdf_size_t new_pos)
{
  pdf_status_t result_status;
  pdf_fsys_disk_file_t work_file = (pdf_fsys_disk_file_t)file->data;

  if (fsetpos(work_file->file_descriptor, (fpos_t*)&new_pos) == 0)
    {
      result_status = PDF_OK;
    }
  else
    {
      switch (errno)
        {
        case EINVAL:
        case ESPIPE:
        case EBADF:
          {
            result_status = PDF_EBADDATA;
            break;
          }
        default:
          {
            /* Other error conditions */
            result_status = PDF_ERROR;
            break;
          }
        }
    }

  return result_status;
}

pdf_bool_t
pdf_fsys_disk_file_can_set_size_p (pdf_fsys_file_t file,
                                   pdf_size_t size)
{
  /* FIXME: Please implement me XD */
  return PDF_TRUE;
}

pdf_size_t
pdf_fsys_disk_file_get_size (pdf_fsys_file_t file)
{
  /* FIXME: Please implement me :) */
  return 0;
}

pdf_status_t
pdf_fsys_disk_file_set_size (pdf_fsys_file_t file,
                             pdf_size_t size)
{
  /* FIXME: Please implement me :D */
  return PDF_OK;
}

pdf_size_t
pdf_fsys_disk_file_read (pdf_fsys_file_t file,
                         const pdf_size_t elem_size,
                         const pdf_size_t elem_count,
                         void *data)
{
  /* FIXME: Please implement me :DDD */
  return 0;
}

pdf_size_t
pdf_fsys_disk_file_write (pdf_fsys_file_t file,
                          const pdf_size_t elem_size,
                          const pdf_size_t elem_count,
                          void *data)
{
  /* FIXME: Please implement me XD */
  return 0;
}

pdf_status_t
pdf_fsys_disk_file_flush (pdf_fsys_file_t file)
{
  pdf_fsys_disk_file_t file_data;
  pdf_status_t result_status;

  file_data = (pdf_fsys_disk_file_t) file->data;

  if (fflush (file_data->file_descriptor) != 0)
    {
      switch (errno)
        {
#ifndef PDF_HOST_WIN32
          /* On Windows platforms (excluding Cygwin), fflush does not
             set errno upon failure. */

        case EBADF:
        case EFAULT:
        case EFBIG:
        case EINVAL:
          {
            /* Bad function parameters to underlying write() */
            result_status = PDF_EBADDATA;
            break;
          }
        case EAGAIN:
          {
            /* non-blocking descriptor and blocking writing
               requested */
            result_status = PDF_EAGAIN;
            break;
          }
        case ENOSPC:
          {
            /* Not room in the disk for the data */
            result_status = PDF_ENOSPC;
            break;
          }
#endif /* !PDF_HOST_WIN32 */
        default:
          {
            /* Other error conditions */
            result_status = PDF_ERROR;
            break;
          }
        }
    }
  else
    {
      result_status = PDF_OK;
    }

  return result_status;
}

pdf_status_t
pdf_fsys_disk_file_request_ria (pdf_fsys_file_t file,
                                pdf_size_t offset,
                                pdf_size_t count)
{
  /* This filesystem implementation do not provide Read-In-Advance
   * capabilities, so this function is a no-op */

  return PDF_OK;
}

pdf_bool_t
pdf_fsys_disk_file_has_ria (pdf_fsys_file_t file)
{
  /* This filesystem implementation do not provide Read-In-Advance
   * capabilities */

  return PDF_FALSE;
}

pdf_status_t
pdf_fsys_disk_file_cancel_ria (pdf_fsys_file_t file)
{
  /* This filesystem implementation do not provide Read-In-Advance
   * capabilities, so this function is a no-op */

  return PDF_OK;
}

pdf_status_t
pdf_fsys_disk_file_close (pdf_fsys_file_t file)
{
  pdf_fsys_disk_file_t file_data;

  file_data = (pdf_fsys_disk_file_t) file->data;

  /* Close the I/O stream */
  if (fclose (file_data->file_descriptor) == EOF)
    {
      /* An error was detected closing the I/O stream */
      return PDF_ERROR;
    }
  
  return PDF_OK;
}

pdf_status_t
pdf_fsys_disk_file_reopen (pdf_fsys_file_t file,
                           enum pdf_fsys_file_mode_e mode)
{
  /* FIXME: Please implement me XD */
  return PDF_OK;
}

/*
 * Private functions
 */

static pdf_status_t
pdf_fsys_disk_get_host_path (pdf_text_t path,
                             pdf_char_t **host_path,
                             pdf_u32_t *host_path_size)
{
  /* Call the pdf_text module to get a host-encoded version of the
     given path */
  return pdf_text_get_host (host_path,
                            host_path_size,
                            path,
                            pdf_text_get_host_encoding ());
}

static pdf_status_t
pdf_fsys_disk_build_mode_string (const enum pdf_fsys_file_mode_e mode,
                                 pdf_char_t *mode_str)
{
  int mode_str_size;

  switch (mode)
    {
    case PDF_FSYS_OPEN_MODE_READ:
      {
        mode_str[0] = 'r';
        mode_str[1] = 0;

        mode_str_size = 1;
        break;
      }
    case PDF_FSYS_OPEN_MODE_WRITE:
      {
        mode_str[0] = 'w';
        mode_str[1] = 0;

        mode_str_size = 1;
        break;
      }
    case PDF_FSYS_OPEN_MODE_RW:
      {
        mode_str[0] = 'r';
        mode_str[1] = '+';
        mode_str[2] = 0;

        mode_str_size = 2;
        break;
      }
    default:
      {
        /* Dummy case to make the compiler happy */
        mode_str[0] = ':';
        mode_str[1] = 'D';
        mode_str[2] = 0;

        mode_str_size = 2;
        break;
      }
    }

#if defined PDF_HOST_WIN32
  /* Windoze portability note:
   *
   * Files are opened in "text mode" (with crlf translation) by
   * default.
   *
   * Although the "b" fopen option is supported by POSIX some old Unix
   * systems may not implement it, so we should use that option to
   * open files only while running in Windows.
   */
  mode_str[mode_str_size] = 'b';
  mode_str[mode_str_size + 1] = 0;
#endif /* PDF_HOST_WIN32 */

  return PDF_OK;
}

static pdf_bool_t
pdf_fsys_disk_win32_device_p (pdf_text_t path)
{
  /* The following special "files", which access devices, exist in all
     directories, case-insensitively, and with all possible endings
     after a period or colon, excpets in pathnames that start with
     \\?\: */
  char *device_names[] = 
    {"NUL", "CON", "PRN", "AUX", "COM1", "COM2", "COM3", "COM4",
     "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3",
     "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};
  int num_device_names = 22;
  int i;
  pdf_bool_t device_p;
  pdf_text_t device_name;
  pdf_status_t ret_code;

  device_p = PDF_FALSE;
  for (i = 0; i < num_device_names; i++)
    {
      pdf_text_new_from_unicode ((pdf_char_t *) device_names[i],
                                 strlen (device_names[i]),
                                 PDF_TEXT_UTF8,
                                 &device_name);
      
      if (pdf_text_cmp (path, device_name, PDF_FALSE, &ret_code) == 0)
        {
          device_p = PDF_TRUE;
          break;
        }

      pdf_text_destroy (device_name);
    }

  return device_p;
}

/* End of pdf-fsys-disk.c */
