/* -*- mode: C -*- Time-stamp: "07/07/14 19:40:46 jemarch"
 *
 *       File:         pdf_stm_f_ahex.c
 *       Author:       Jose E. Marchesi (jemarch@gnu.org)
 *       Date:         Fri Jul 13 17:08:41 2007
 *
 *       GNU PDF Library - ASCII Hex decoder filter
 *
 */

#include <config.h>
#include <malloc.h>
#include <xalloc.h>
#include <string.h>
#include <stdio.h>
#include <pdf_stm_f_ahex.h>

static int pdf_stm_f_ahex_white_p (int hex);
static int pdf_stm_f_ahex_hex_p (int hex);
static int pdf_stm_f_ahex_hex2int (int hex);
static pdf_char_t pdf_stm_f_ahex_int2hex (int n);
static int pdf_stm_f_ahex_encode (pdf_char_t *in, pdf_stm_pos_t in_size,
                                  pdf_char_t **out, pdf_stm_pos_t *out_size);
static int pdf_stm_f_ahex_decode (pdf_char_t *in, pdf_stm_pos_t in_size,
                                  pdf_char_t **out, pdf_stm_pos_t *out_size);



int
pdf_stm_f_ahex_init (void **filter_data,
                     void *conf_data)
{
  pdf_stm_f_ahex_data_t *data;
  pdf_stm_f_ahex_conf_t conf;

  data = (pdf_stm_f_ahex_data_t *) filter_data;
  conf = (pdf_stm_f_ahex_conf_t) conf_data;

  /* Create the private data storage */
  *data =
    (pdf_stm_f_ahex_data_t) xmalloc (sizeof(struct pdf_stm_f_ahex_data_s));
  (*data)->mode = conf->mode;

  return PDF_OK;
}

int 
pdf_stm_f_ahex_apply (void *filter_data,
                       pdf_char_t *in, pdf_stm_pos_t in_size,
                       pdf_char_t **out, pdf_stm_pos_t *out_size)
{
  pdf_stm_f_ahex_data_t data;

  data = (pdf_stm_f_ahex_data_t) filter_data;
  switch (data->mode)
    {
    case PDF_STM_F_AHEX_MODE_ENCODE:
      {
        return pdf_stm_f_ahex_encode (in, in_size, out, out_size);
      }
    case PDF_STM_F_AHEX_MODE_DECODE:
      {
        return pdf_stm_f_ahex_decode (in, in_size, out, out_size);
      }
    default:
      {
        return PDF_ERROR;
      }
    }
}

int
pdf_stm_f_ahex_dealloc (void **filter_data)
{
  pdf_stm_f_ahex_data_t *data;
  
  data = (pdf_stm_f_ahex_data_t *) filter_data;
  free (*data);

  return PDF_OK;
}

/* Private functions */

static int
pdf_stm_f_ahex_white_p (int hex)
{
  return ((hex == '\0') || /* Null */
          (hex == '\t') || /* Tab */
          (hex == '\n') || /* Line feed */
          (hex == '\f') || /* Form feed */
          (hex == '\r') || /* Carriage return */
          (hex == 32));    /* Space character */
}

static int
pdf_stm_f_ahex_hex_p (int hex)
{
  return (((hex >= 'a') && (hex <= 'f')) ||
          ((hex >= 'A') && (hex <= 'F')) ||
          ((hex >= '0') && (hex <= '9')));
}

static const pdf_char_t to_hex[16] = "0123456789ABCDEF";

static pdf_char_t
pdf_stm_f_ahex_int2hex (int hex)
{
  return to_hex[hex & 0x0f];
}

static int
pdf_stm_f_ahex_hex2int (int hex)
{
  if ((hex >= 'a') && (hex <= 'f'))
    {
      return (hex - 'a') + 0xA;
    }
  if ((hex >= 'A') && (hex <= 'F'))
    {
      return (hex - 'A') + 0xA;
    }
  if ((hex >= '0') && (hex <= '9'))
    {
      return (hex - '0');
    }
}

#define AHEX_ENC_LINE_LENGTH 80

static int
pdf_stm_f_ahex_encode (pdf_char_t *in, 
                       pdf_stm_pos_t in_size,
                       pdf_char_t **out,
                       pdf_stm_pos_t *out_size)
{
  pdf_stm_pos_t pos_in;
  pdf_stm_pos_t pos_out;
  pdf_stm_pos_t num_lines;
  pdf_stm_pos_t line_length;

  *out_size = 
    (in_size * 2) +                         /* Expansion of 1:2 */
    1 +                                     /* EOD marker */
    ((in_size * 2) / AHEX_ENC_LINE_LENGTH); /* line splitting */
  *out = (pdf_char_t *) xmalloc (*out_size);

  pos_out = 0;
  num_lines = 0;
  line_length = 0;
  for (pos_in = 0;
       pos_in < in_size;
       pos_in++)
    {
      pos_out = (pos_in * 2) + num_lines;
      
      (*out)[pos_out] = pdf_stm_f_ahex_int2hex (in[pos_in] >> 4);
      (*out)[pos_out + 1] = pdf_stm_f_ahex_int2hex (in[pos_in]);

      line_length = line_length + 2;

      if (line_length >= AHEX_ENC_LINE_LENGTH)
        {
          (*out)[pos_out + 2] = '\n';
          line_length = 0;
          num_lines++;
        }
    }
  if (line_length == 0)
    {
      pos_out = pos_out + 3;
    }
  else
    {
      pos_out = pos_out + 2;
    }
  (*out)[pos_out] = '>';

  return PDF_OK;
}

static int
pdf_stm_f_ahex_decode (pdf_char_t *in,
                       pdf_stm_pos_t in_size,
                       pdf_char_t **out,
                       pdf_stm_pos_t *out_size)
{
  

  return PDF_OK;
}


/* End of pdf_stm_f_ahex.c */
