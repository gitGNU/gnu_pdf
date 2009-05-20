#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "pdf.h"

static char *fmtbin(const char *data, int size)
{
  int i;
  char *ret, *retp;

  if (size == -1)
    size = strlen(data);

  ret = malloc(3 + (size*4));
  if (!ret) abort();

  retp = ret;
  (*retp++) = '"';
  for (i = 0; i < size; ++i)
    {
      unsigned char ch = data[i];
      if (ch == '"' || ch == '\\')
        {
          *(retp++) = '\\';
          *(retp++) = ch;
        }
      else if (data[i] >= 32 && data[i] < 127)
        *(retp++) = ch;
      else
        {
          sprintf(retp, "\\x%02x", (int)ch);
          retp += 4;
        }
    }
  (*retp++) = '"';
  (*retp++) = '\0';
  return ret;
}

static void print_tok(pdf_token_t token)
{
  char tmpbuf[256];
  const char *typ, *str = NULL;
  int strsize = -1;
  pdf_size_t i;

  switch(pdf_token_get_type(token))
    {
    case PDF_TOKEN_INTEGER:
      typ = "INTEGER";
      snprintf(tmpbuf, sizeof(tmpbuf), "%d", pdf_token_get_integer_value(token));
      str = tmpbuf;
      break;

    case PDF_TOKEN_REAL:
      typ = "REAL";
      snprintf(tmpbuf, sizeof(tmpbuf), "%f", pdf_token_get_real_value(token));
      str = tmpbuf;
      break;

    case PDF_TOKEN_STRING:
      typ = "STRING";
      str = pdf_token_get_string_data(token);
      strsize = pdf_token_get_string_size(token);
      break;

    case PDF_TOKEN_NAME:
      typ = "NAME";
      str = pdf_token_get_name_data(token);
      strsize = pdf_token_get_name_size(token);
      break;

    case PDF_TOKEN_COMMENT:
      typ = "COMMENT";
      str = pdf_token_get_comment_data(token);
      strsize = pdf_token_get_comment_size(token);
      break;

    case PDF_TOKEN_KEYWORD:
      typ = "KEYWORD";
      str = pdf_token_get_keyword_data(token);
      strsize = pdf_token_get_keyword_size(token);
      break;

    case PDF_TOKEN_DICT_START:
      typ = "DICT_START";
      break;
    case PDF_TOKEN_DICT_END:
      typ = "DICT_END";
      break;
    case PDF_TOKEN_ARRAY_START:
      typ = "ARRAY_START";
      break;
    case PDF_TOKEN_ARRAY_END:
      typ = "ARRAY_END";
      break;
    case PDF_TOKEN_PROC_START:
      typ = "PROC_START";
      break;
    case PDF_TOKEN_PROC_END:
      typ = "PROC_END";
      break;

    default:
      typ = "[unknown]";
      sprintf(tmpbuf, "%d", pdf_token_get_type(token));
      str = tmpbuf;
    }

  if (str == NULL)
    {
      tmpbuf[0] = '\0';
      str = tmpbuf;
    }
  if (str != tmpbuf) str = fmtbin(str, strsize);
  printf("%s(%s)\n", typ, str);
  if (str != tmpbuf)
    {
      free((void*)str);
    }
};

void print_file(FILE *file)
{
  pdf_status_t rv;
  pdf_token_reader_t reader = NULL;
  pdf_token_t token;
  pdf_stm_t stm = NULL;

  rv = pdf_stm_cfile_new (file, 0, 0 /*cache_size*/, PDF_STM_READ, &stm);
  if (rv != PDF_OK)
    {
      fprintf(stderr, "failed to create stream\n");
      goto out;
    }

  rv = pdf_token_reader_new(stm, &reader);
  if (rv != PDF_OK)
    {
      fprintf(stderr, "failed to create reader\n");
      goto out;
    }

  while (( rv = pdf_token_read(reader, 0, &token) ) == PDF_OK)
    {
      print_tok(token);
      pdf_token_destroy(token);
    }

  if (rv != PDF_EEOF)
    {
      fprintf(stderr, "read_token error %d\n", rv);
      goto out;
    }

  fprintf(stderr, "done\n");
out:
  if (reader) pdf_token_reader_destroy(reader);
  if (stm) pdf_stm_destroy(stm);
}

int main(int argc, char **argv)
{
  setlocale(LC_ALL, "");
  print_file(stdin);
  return 0;
}
