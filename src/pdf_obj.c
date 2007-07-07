/* -*- mode: C -*- Time-stamp: "07/07/07 05:47:44 jemarch"
 *
 *       File:         pdf_obj.c
 *       Author:       Jose E. Marchesi (jemarch@gnu.org)
 *       Date:         Sat Jul  7 03:04:30 2007
 *
 *       GNU PDF Library - PDF objects implementation
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

#include <string.h>
#include <pdf_obj.h>

/* Private functions prototypes */

static pdf_obj_t pdf_alloc_obj (void);
static void pdf_dealloc_obj (pdf_obj_t obj);
static pdf_dict_entry_t pdf_alloc_dict_entry (void);
static void pdf_dealloc_dict_entry (pdf_dict_entry_t entry);

/* Object creation */

pdf_obj_t
pdf_create_null (void)
{
  pdf_obj_t null_obj;

  null_obj = pdf_alloc_obj ();
  null_obj->type = PDF_NULL_OBJ;

  return null_obj;
}

pdf_obj_t
pdf_create_boolean (int value)
{
  pdf_obj_t bool_obj;

  bool_obj = pdf_alloc_obj ();
  bool_obj->type = PDF_BOOLEAN_OBJ;
  bool_obj->value.boolean = value;

  return bool_obj;
}

pdf_obj_t
pdf_create_int (int value)
{
  pdf_obj_t int_obj;

  int_obj = pdf_alloc_obj ();
  int_obj->type = PDF_INT_OBJ;
  int_obj->value.integer = value;

  return int_obj;
}

pdf_obj_t
pdf_create_real (float value)
{
  pdf_obj_t real_obj;

  real_obj = pdf_alloc_obj ();
  real_obj->type = PDF_REAL_OBJ;
  real_obj->value.real = value;

  return real_obj;
}

pdf_obj_t
pdf_create_string (char *value,
                   int size)
{
  pdf_obj_t string_obj;

  string_obj = pdf_alloc_obj ();
  string_obj->type = PDF_STRING_OBJ;
  string_obj->value.string.data = 
    (char *) xmalloc (size);
  memcpy (string_obj->value.string.data,
          value,
          size);
  string_obj->value.string.size = size;

  return string_obj;
}

pdf_obj_t
pdf_create_name (char *value,
                 int size)
{
  pdf_obj_t name_obj;

  name_obj = pdf_alloc_obj ();
  name_obj->type = PDF_NAME_OBJ;
  name_obj->value.name.data =
    (char *) xmalloc (size);
  memcpy (name_obj->value.name.data,
          value,
          size);
  name_obj->value.name.size = size;

  return name_obj;
}

pdf_obj_t
pdf_create_array (void)
{
  pdf_obj_t new_array;

  new_array = pdf_alloc_obj ();
  new_array->type = PDF_ARRAY_OBJ;
  /*  new_array->value.array.objs = */

  return new_array;
}

pdf_obj_t
pdf_create_dict (void)
{
  pdf_obj_t new_dict;

  new_dict = pdf_alloc_obj ();
  new_dict->type = PDF_DICT_OBJ;
  /*  new_dict->value.dict.entries = */

  return new_dict;
}

pdf_obj_t
pdf_create_indirect (unsigned int on,
                     unsigned int gn)
{
  pdf_obj_t new_indirect;

  new_indirect = pdf_alloc_obj ();
  new_indirect->type = PDF_INDIRECT_OBJ;
  new_indirect->value.indirect.on = on;
  new_indirect->value.indirect.gn = gn;

  return new_indirect;
}

/* Private functions */

static pdf_obj_t
pdf_alloc_obj (void)
{
  pdf_obj_t new_obj;

  new_obj = (pdf_obj_t) xmalloc (sizeof(struct pdf_obj_s));
  return new_obj;
}

static void
pdf_dealloc_obj (pdf_obj_t obj)
{
  switch (obj->type)
    {
    case PDF_STRING_OBJ:
      {
        free (obj->value.string.data);
        break;
      }
    case PDF_NAME_OBJ:
      {
        free (obj->value.name.data);
        break;
      }
    case PDF_ARRAY_OBJ:
      {
        /* obj->value.array.objs */
        break;
      }
    case PDF_DICT_OBJ:
      {
        /* obj->value.dict.entries */
        break;
      }
    }

  free (obj);
}


static pdf_dict_entry_t 
pdf_alloc_dict_entry (void)
{
  pdf_dict_entry_t entry;

  entry = (pdf_dict_entry_t) xmalloc (sizeof(struct pdf_dict_entry_s));
  return entry;
}

static void 
pdf_dealloc_dict_entry (pdf_dict_entry_t entry)
{
  free (entry);
}


/* End of pdf_obj.c */
