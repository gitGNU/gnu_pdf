/* -*- mode: C -*- Time-stamp: "08/05/22 20:38:02 jemarch"
 *
 *       File:         pdf-fsys.c
 *       Date:         Thu May 22 15:51:13 2008
 *
 *       GNU PDF Library - Filesystem Module
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

#include <pdf-alloc.h>
#include <pdf-fsys.h>
#include <pdf-fsys-def.h>

/* Private procedures declaration */
static pdf_fsys_t pdf_fsys_alloc (void);
static void pdf_fsys_dealloc (pdf_fsys_t filesystem);

/*
 * Filesystem Interface Implementation
 */

pdf_size_t
pdf_fsys_get_free_space (pdf_fsys_t filesystem,
                         pdf_text_t path_name)
{
  if (filesystem == NULL)
    {
      /* Use the default filesystem */
      return 
        pdf_fsys_def_get_free_space(path_name);
    }
  else
    {
      return 
        filesystem->implementation->get_free_space_fn (path_name);
    }
}

pdf_status_t
pdf_fsys_open (const pdf_fsys_t filesystem,
               const pdf_text_t path_name,
               const enum pdf_fsys_file_mode_e mode,
               pdf_fsys_file_t file)
{
  if (filesystem == NULL)
    {
      /* Use the default filesystem */
      return
        pdf_fsys_def_open(path_name, mode, file);
    }
  else
    {
      return filesystem->implementation->open_fn (path_name,
                                                  mode,
                                                  file);
    }
}

pdf_status_t 
pdf_fsys_create_folder (const pdf_fsys_t filesystem,
                        const pdf_text_t path_name)
{
  if (filesystem == NULL)
    {
      /* Use the default filesystem */
      return
        pdf_fsys_def_create_folder(path_name);
    }
  else
    {
      return 
        filesystem->implementation->create_folder_fn (path_name);
    }
}

pdf_status_t
pdf_fsys_get_folder_contents (const pdf_fsys_t filesystem,
                              const pdf_text_t path_name,
                              pdf_list_t item_list)
{
  if (filesystem == NULL)
    {
      /* Use the default filesystem */
      return 
        pdf_fsys_def_get_folder_contents(path_name, item_list);
    }
  else
    {
      return
        filesystem->implementation->get_folder_contents_fn (path_name,
                                                            item_list);
    }
}

pdf_status_t
pdf_fsys_get_parent (const pdf_fsys_t filesystem,
                     const pdf_text_t path_name,
                     pdf_text_t parent_path)
{
  if (filesystem == NULL)
    {
      /* Use the default filesystem */
      return
        pdf_fsys_def_get_parent(path_name, parent_path);
    }
  else
    {
      return
        filesystem->implementation->get_parent_fn (path_name,
                                                   parent_path);
    }
}

pdf_status_t 
pdf_fsys_remove_folder (const pdf_fsys_t filesystem,
                        const pdf_text_t path_name)
{
  if (filesystem == NULL)
    {
      /* Use the default filesystem */
      return
        pdf_fsys_def_remove_folder(path_name);
    }
  else
    {
      return 
        filesystem->implementation->remove_folder_fn (path_name);
    }
}

pdf_status_t 
pdf_fsys_get_item_props (pdf_fsys_t filesystem,
                         pdf_text_t path_name,
                         struct pdf_fsys_item_props_s *item_props)
{
  if (filesystem == NULL)
    {
      /* Use the default filesystem */
      return
        pdf_fsys_def_get_item_props(path_name, item_props);
    }
  else
    {
      return
        filesystem->implementation->get_item_props_fn (path_name,
                                                       item_props);
    }
}

pdf_status_t
pdf_fsys_item_props_to_hash (const struct pdf_fsys_item_props_s item_props,
                             pdf_hash_t props_hash)
{
  pdf_bool_t *is_hidden; 
  pdf_bool_t *is_read_only;
  pdf_bool_t *is_checked_out;
  pdf_bool_t *is_published;
  /* FIXME: */
  /*  pdf_time_t creation_date; */
  /*  pdf_time_t mod_date; */
  pdf_u32_t *file_size_high;
  pdf_u32_t *file_size_low;
  pdf_u32_t *folder_size;
  pdf_u32_t *creator_code;
  pdf_u32_t *type_code;
  pdf_u32_t *version_major;
  pdf_u32_t *version_minor;

  /* Allocate memory for the hash values */
  is_hidden = (pdf_bool_t *) pdf_alloc (sizeof(pdf_bool_t));
  is_read_only = (pdf_bool_t *) pdf_alloc (sizeof(pdf_bool_t));
  is_checked_out = (pdf_bool_t *) pdf_alloc (sizeof(pdf_bool_t));
  is_published = (pdf_bool_t *) pdf_alloc (sizeof(pdf_bool_t));
  file_size_high = (pdf_u32_t*) pdf_alloc (sizeof(pdf_u32_t));
  file_size_low = (pdf_u32_t*) pdf_alloc (sizeof(pdf_u32_t));
  folder_size = (pdf_u32_t*) pdf_alloc (sizeof(pdf_u32_t));
  creator_code = (pdf_u32_t*) pdf_alloc (sizeof(pdf_u32_t));
  type_code = (pdf_u32_t*) pdf_alloc (sizeof(pdf_u32_t));
  version_major = (pdf_u32_t*) pdf_alloc (sizeof(pdf_u32_t));
  version_minor = (pdf_u32_t*) pdf_alloc (sizeof(pdf_u32_t));
  
  /* Get the values from the props structure */
  *is_hidden = item_props.is_hidden;
  *is_read_only = item_props.is_read_only;
  *is_checked_out = item_props.is_checked_out;
  *is_published = item_props.is_published;
  *file_size_high = item_props.file_size_high;
  *file_size_low = item_props.file_size_low;
  *folder_size = item_props.folder_size;
  *creator_code = item_props.creator_code;
  *type_code = item_props.type_code;
  *version_major = item_props.version_major;
  *version_minor = item_props.version_minor;

  /* Associate values with hash keys */
  if (pdf_hash_add (props_hash, "isHidden", (void *) is_hidden) !=
      PDF_OK)
    {
      return PDF_ERROR;
    }
  if (pdf_hash_add (props_hash, "isReadOnly", (void *) is_read_only) !=
      PDF_OK)
    {
      return PDF_ERROR;
    }
  if (pdf_hash_add (props_hash, "isCheckedOut", (void *) is_checked_out) !=
      PDF_OK)
    {
      return PDF_ERROR;
    }
  if (pdf_hash_add (props_hash, "isPublished", (void *) is_published) !=
      PDF_OK)
    {
      return PDF_ERROR;
    }
  if (pdf_hash_add (props_hash, "fileSizeHigh", (void *) file_size_high) !=
      PDF_OK)
    {
      return PDF_ERROR;
    }
  if (pdf_hash_add (props_hash, "fileSizeLow", (void *) file_size_low) !=
      PDF_OK)
    {
      return PDF_ERROR;
    }
  if (pdf_hash_add (props_hash, "folderSize", (void *) folder_size) !=
      PDF_OK)
    {
      return PDF_ERROR;
    }
  if (pdf_hash_add (props_hash, "creatorCode", (void *) creator_code) !=
      PDF_OK)
    {
      return PDF_ERROR;
    }
  if (pdf_hash_add (props_hash, "typeCode", (void *) type_code) !=
      PDF_OK)
    {
      return PDF_ERROR;
    }
  if (pdf_hash_add (props_hash, "versionMajor", (void *) version_major) !=
      PDF_OK)
    {
      return PDF_ERROR;
    }
  if (pdf_hash_add (props_hash, "versionMinor", (void *) version_minor) !=
      PDF_OK)
    {
      return PDF_ERROR;
    }

  /* Done */
  return PDF_OK;
}

pdf_bool_t
pdf_fsys_item_p (pdf_fsys_t filesystem,
                 pdf_text_t path_name)
{
  if (filesystem == NULL)
    {
      /* Use the default filesystem */
      return
        pdf_fsys_def_item_p(path_name);
    }
  else
    {
      return 
        filesystem->implementation->item_p_fn (path_name);
    }
}

pdf_bool_t
pdf_fsys_item_readable_p (pdf_fsys_t filesystem,
                          pdf_text_t path_name)
{
  if (filesystem == NULL)
    {
      /* Use the default filesystem */
      return
        pdf_fsys_def_item_readable_p(path_name);
    }
  else
    {
      return
        filesystem->implementation->item_readable_p_fn (path_name);
    }
}

pdf_bool_t
pdf_fsys_item_writable_p (pdf_fsys_t filesystem,
                          pdf_text_t path_name)
{
  if (filesystem == NULL)
    {
      /* Use the default filesystem */
      return
        pdf_fsys_def_item_writable_p(path_name);
    }
  else
    {
      return
        filesystem->implementation->item_writable_p_fn (path_name);
    }
}

pdf_text_t
pdf_fsys_get_temp_path_name (pdf_fsys_t filesystem)
{
  if (filesystem == NULL)
    {
      /* Use the default filesystem */
      return
        pdf_fsys_def_get_temp_path_name();
    }
  else
    {
      return 
        filesystem->implementation->get_temp_path_name_fn ();
    }
}

/*
 * File Interface Implementation
 */

pdf_fsys_t 
pdf_fsys_file_get_filesystem (pdf_fsys_file_t file)
{
  /* May be NULL if it is the default filesystem */
  return file->fs;
}

enum pdf_fsys_file_mode_e
pdf_fsys_file_get_mode (pdf_fsys_file_t file)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_get_mode(file);
    }
  else
    {
      return 
        (file->fs->implementation->file_get_mode_fn) (file);
    }
}

pdf_text_t
pdf_fsys_file_get_url (pdf_fsys_file_t file)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_get_url(file);
    }
  else
    {
      return
        (file->fs->implementation->file_get_url_fn) (file);
    }
}

enum pdf_fsys_file_mode_e
pdf_fsys_file_set_mode (pdf_fsys_file_t file,
                        enum pdf_fsys_file_mode_e new_mode)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_set_mode(file, new_mode);
    }
  else
    {
      return
        (file->fs->implementation->file_set_mode_fn) (file,
                                                      new_mode);
    }
}

pdf_bool_t 
pdf_fsys_file_same_p (pdf_fsys_file_t file,
                      pdf_text_t path)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_same_p(file, path);
    }
  else
    {
      return
        (file->fs->implementation->file_same_p_fn) (file, path);
    }
}

pdf_status_t
pdf_fsys_file_get_pos (pdf_fsys_file_t file,
                       pdf_size_t *pos)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_get_pos(file, pos);
    }
  else
    {
      return
        (file->fs->implementation->file_get_pos_fn) (file, pos);
    }
}

pdf_status_t
pdf_fsys_file_set_pos (pdf_fsys_file_t file,
                       pdf_size_t new_pos)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_set_pos(file, new_pos);
    }
  else
    {
      return 
        (file->fs->implementation->file_set_pos_fn) (file,
                                                     new_pos);
    }
}

pdf_bool_t
pdf_fsys_file_can_set_size_p (pdf_fsys_file_t file,
                              pdf_size_t size)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_can_set_size_p(file, size);
    }
  else
    {
      return
        (file->fs->implementation->file_can_set_size_p_fn) (file,
                                                            size);
    }
}

pdf_status_t
pdf_fsys_file_get_size (pdf_fsys_file_t file, 
                        pdf_size_t *size)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_get_size(file, size);
    }
  else
    {
      return
        (file->fs->implementation->file_get_size_fn) (file,
                                                      size);
    }
}

pdf_status_t
pdf_fsys_file_set_size (pdf_fsys_file_t file,
                        pdf_size_t size)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_set_size(file, size);
    }
  else
    {
      return
        (file->fs->implementation->file_set_size_fn) (file,
                                                      size);
    }
}

pdf_size_t 
pdf_fsys_file_read (pdf_fsys_file_t file,
                    const pdf_size_t elem_size,
                    const pdf_size_t elem_count,
                    void *data)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_read(file, elem_size, elem_count, data);
    }
  else
    {
      return
        (file->fs->implementation->file_read_fn) (file,
                                                  elem_size,
                                                  elem_count,
                                                  data);
    }
}

pdf_size_t
pdf_fsys_file_write (pdf_fsys_file_t file,
                     const pdf_size_t elem_size,
                     const pdf_size_t elem_count,
                     void *data)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_write(file, elem_size, elem_count, data);
    }
  else
    {
      return
        (file->fs->implementation->file_write_fn) (file,
                                                   elem_size,
                                                   elem_count,
                                                   data);
    }
}

pdf_status_t
pdf_fsys_file_flush (pdf_fsys_file_t file)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_flush(file);
    }
  else
    {
      return
        (file->fs->implementation->file_flush_fn) (file);
    }
}

pdf_status_t
pdf_fsys_file_request_ria (pdf_fsys_file_t file,
                           pdf_size_t offset,
                           pdf_size_t count)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_request_ria(file, offset, count);
    }
  else
    {
      return
        (file->fs->implementation->file_request_ria_fn) (file,
                                                         offset,
                                                         count);
    }
}

pdf_bool_t 
pdf_fsys_file_has_ria (pdf_fsys_file_t file)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_has_ria(file);
    }
  else
    {
      return
        (file->fs->implementation->file_has_ria_fn) (file);
    }
}

pdf_status_t
pdf_fsys_file_cancel_ria (pdf_fsys_file_t file)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_cancel_ria(file);
    }
  else
    {
      return
        (file->fs->implementation->file_cancel_ria_fn) (file);
    }
}

pdf_status_t
pdf_fsys_file_close (pdf_fsys_file_t file)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_close(file);
    }
  else
    {
      return
        (file->fs->implementation->file_close_fn) (file);
    }
}

pdf_status_t
pdf_fsys_file_reopen (pdf_fsys_file_t file,
                      enum pdf_fsys_file_mode_e mode)
{
  if (file->fs == NULL)
    {
      /* Use the default filesystem */
      return pdf_fsys_def_file_reopen(file, mode);
    }
  else
    {
      return
        (file->fs->implementation->file_reopen_fn) (file,
                                                    mode);
    }
}

/*
 * Filesystem Definition Interface Implementation
 */

pdf_fsys_t
pdf_fsys_create (struct pdf_fsys_impl_s implementation)
{
  pdf_fsys_t filesystem;
  struct pdf_fsys_impl_s *own_implementation;

  /* Allocate a new filesystem */
  filesystem = pdf_fsys_alloc ();

  /* Allocate a new implementation structure */
  own_implementation = (struct pdf_fsys_impl_s *) 
    pdf_alloc (sizeof(struct pdf_fsys_impl_s));

  /* Set its properties */
  *(filesystem->implementation) = implementation;

  return filesystem;
}

pdf_status_t
pdf_fsys_destroy (pdf_fsys_t filesystem)
{
  /* Deallocate all resources used by the filesystem implementation */
  pdf_dealloc (filesystem->implementation);
  pdf_fsys_dealloc (filesystem);

  return PDF_OK;
}

/*
 * Private functions
 */

static pdf_fsys_t
pdf_fsys_alloc (void)
{
  pdf_fsys_t filesystem;
  
  filesystem = 
    (pdf_fsys_t) pdf_alloc (sizeof(struct pdf_fsys_s));
  return filesystem;
}

static void
pdf_fsys_dealloc (pdf_fsys_t filesystem)
{
  pdf_dealloc (filesystem);
}

/* End of pdf-fsys.c */
