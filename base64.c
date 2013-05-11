/****************
MySQL UDF

gcc -Wall -I/usr/local/include -shared -o base64.so -c base64.c

SELECT base64encode('data,binary,text,...');
SELECT base64decode('b64strings');

this code based on PHP's base64.c
term is below.

Murakami Takeshi
takeshi@softagency.co.jp

--- changelog
2001-06-18

   * base64decode can handle LF,CR,'\'

  -- takeshi@softagency.co.jp 

2001-06-15

   * init.

  -- takeshi@softagency.co.jp 

****************/
/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jim Winstead (jimw@php.net)                                  |
   +----------------------------------------------------------------------+
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>

static char base64_table[] =
{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
};
static char base64_pad = '=';

typedef unsigned char  uchar;

#define is_base64char(c) ( ( (uchar)0x30 <= (uchar)(c) && (uchar)(c) <= (uchar)0x39 ) || \
                           ( (uchar)0x41 <= (uchar)(c) && (uchar)(c) <= (uchar)0x5a ) || \
                           ( (uchar)0x61 <= (uchar)(c) && (uchar)(c) <= (uchar)0x7a ) || \
                           ( (uchar)0x2b == (uchar)(c) ) || \
                           ( (uchar)0x2f == (uchar)(c) ) )


/*****************************/
my_bool
base64encode_init(UDF_INIT* initid, UDF_ARGS* args, char* message)
{
  if (args->arg_count != 1)
  {
    strcpy(message,"Wrong arguments to base64encode;  must be (str)");
    return 1;
  }

  if ( args->arg_type[0] != STRING_RESULT ) {
    args->arg_type[0] = STRING_RESULT;
  }

  return 0;
}

void
base64encode_deinit(UDF_INIT* initid)
{
  if (initid->ptr)
    free(initid->ptr);
}


char
*base64encode(UDF_INIT *initid, UDF_ARGS *args, char *result,
               unsigned long *ret_length, char *is_null, char *error)
{
  const char *current;
  unsigned long i = 0;
  unsigned long len;

  /* *is_null=0; */
  current = args->args[0];
  len = args->lengths[0];

  if (len <= 0  ||  args->arg_type[0] != STRING_RESULT )
  {
    *is_null=0;
    *ret_length = 0;
    return 0;
  }

  if (!args || !args->args[0])
  {
    *is_null=1;
    *ret_length = 0;
    return 0;
  }

  if ( len < 1)  { len=1; }

  if (!( result = (char*) malloc( sizeof(char) * ((len + 3 - len % 3) * 4 / 3 + 1) ) ) )
  {
    /* strmov(message,"Couldn't allocate memory in base64encode_init"); */
    *is_null=1;
    *error=1;
    *ret_length = 0;
    return 0;
  }
  if (initid->ptr) free(initid->ptr);
  initid->ptr = result;

  i=0;
  while (len > 2)
  { /* keep going until we have less than 24 bits */
    result[i++] = base64_table[current[0] >> 2];
    result[i++] = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
    result[i++] = base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
    result[i++] = base64_table[current[2] & 0x3f];

    current += 3;
    len -= 3; /* we just handle 3 octets of data */
  }

  /* now deal with the tail end of things */
  if (len != 0)
  {
    result[i++] = base64_table[current[0] >> 2];
    if (len > 1) {
      result[i++] = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
      result[i++] = base64_table[(current[1] & 0x0f) << 2];
      result[i++] = base64_pad;
    }
    else {
      result[i++] = base64_table[(current[0] & 0x03) << 4];
      result[i++] = base64_pad;
      result[i++] = base64_pad;
    }
  }

    *ret_length = i;

  result[i] = '\0';
  return result;
}

/*****************************/
my_bool
base64decode_init(UDF_INIT* initid, UDF_ARGS* args, char* message)
{
  if (args->arg_count != 1)
  {
    strcpy(message,"Wrong arguments to base64decode;  must be (str)");
    return 1;
  }

  if ( args->arg_type[0] != STRING_RESULT ) {
    args->arg_type[0] = STRING_RESULT;
  }

  return 0;
}

void
base64decode_deinit(UDF_INIT* initid)
{
  if (initid->ptr)
    free(initid->ptr);
}

char
*base64decode(UDF_INIT *initid, UDF_ARGS *args, char *result,
               unsigned long *ret_length, char *is_null, char *error)
{
  const char *current;
  unsigned long i, j, k, ch;
  unsigned long len;
  static short reverse_table[256];
  static int table_built;

  if (++table_built == 1) {
    char *chp;
    for(ch = 0; ch < 256; ch++) {
      chp = strchr(base64_table, ch);
      if(chp) {
	reverse_table[ch] = chp - base64_table;
      } else {
	reverse_table[ch] = -1;
      }
    }
  }

  current = args->args[0];
  len = args->lengths[0];

  if (len <= 0  ||  args->arg_type[0] != STRING_RESULT )
  {
    *is_null=0;
    *ret_length = 0;
    return 0;
  }

  if (!args || !args->args[0])
  {
    *is_null=1;
    *ret_length = 0;
    return 0;
  }

  if ( len < 1)  { len=1; }

  if (!( result = (char*) malloc( sizeof(char) * (len + 1) ) ) )
  {
    *is_null=1;
    *error=1;
    *ret_length = 0;
    return 0;
  }
  if (initid->ptr) free(initid->ptr);
  initid->ptr = result;

  i=0; j=0;
  while ((ch = *current) != '\0')
  {
    current++;

    if (ch == base64_pad) break;

    if ( ! is_base64char(ch) ) { continue; }
    if ( ch == 0x5c )
    {
       if (  *current != '\0' ) {
          current++;
          continue;
       }
       else  {
          break;
       }
    }

    if (ch == ' ') ch = '+'; 

    ch = reverse_table[ch];
    if (ch < 0) continue;

    switch(i % 4)
    {
      case 0:
	result[j] = ch << 2;
	break;
      case 1:
	result[j++] |= ch >> 4;
	result[j] = (ch & 0x0f) << 4;
	break;
      case 2:
	result[j++] |= ch >>2;
	result[j] = (ch & 0x03) << 6;
	break;
      case 3:
	result[j++] |= ch;
	break;
    }
    i++;
  }

  k = j;
  /* mop things up if we ended on a boundary */
  if (ch == base64_pad)
  {
    switch(i % 4)
    {
      case 0:
      case 1:
	*is_null=1;
	*error=1;
	*ret_length = 0;
	return 0;
      case 2:
	k++;
      case 3:
	result[k++] = 0;
    }
  }

  *ret_length = j;

  result[k] = '\0';
  return result;
}
