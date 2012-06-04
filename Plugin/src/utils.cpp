#ifdef XP_WIN
#include <windows.h>
#include <windowsx.h>
#endif


#ifdef XP_UNIX
#include <string.h>
#endif

#include <sql.h>
#include <sqlext.h>
#include <wchar.h>

#include "plugin.h"
#include "mthreads.h"
#include "npfunctions.h"
//#include "prtypes.h"

#include "utils.h"

#define UNICODE



static size_t
calc_len_for_utf8 (const wchar_t *str)
{
  size_t len = 0;
  wchar_t c;

  if (!str)
    return len;

  while ((c = *str))
    {
      if (c < 0x80)
        len += 1;
      else if (c < 0x800)
        len += 2;
      else if (c < 0x10000)
        len += 3;
      else if (c < 0x200000)
        len += 4;
      else
        len += 1;

      str++;
    }
  return len;
}


static size_t
utf8_len (const char *p, int size=-1)
{
  size_t len = 0;

  if (!*p)
    return 0;

  if (size == -1) 
    while (*p)
    {
      for (p++; (*p & 0xC0) == 0x80; p++)
	  ;
      len++;
    }
  else
    while (size > 0)
      {
	for (p++, size--; (size > 0) && ((*p & 0xC0) == 0x80); p++, size--)
	  ;
	len++;
      }
  return len;
}


/*
 *  size      - size of buffer for output utf8 string in bytes
 *  return    - length of output utf8 string
 */
static size_t
wcstoutf8 (const wchar_t *wstr, char *ustr, size_t size)
{
  size_t len;
  wchar_t c;
  int first;
  size_t i;
  size_t count = 0;

  if (!wstr)
    return 0;

  while (count < size && (c = *wstr))
    {
      if (c < 0x80)
	{
	  len = 1;
	  first = 0;
	}
      else if (c < 0x800)
	{
	  len = 2;
	  first = 0xC0;
	}
      else if (c < 0x10000)
	{
	  len = 3;
	  first = 0xE0;
	}
      else if (c < 0x200000)
	{
	  len = 4;
	  first = 0xf0;
	}
      else
	{
	  len = 1;
	  first = 0;
	  c = '?';
	}

      if (size - count < len)
	{
	  return count;
	}

      for (i = len - 1; i > 0; --i)
	{
	  ustr[i] = (c & 0x3f) | 0x80;
	  c >>= 6;
	}
      ustr[0] = c | first;

      ustr += len;
      count += len;
      wstr++;
    }
  return count;
}



char *
strdup_WtoU8 (const wchar_t *str)
{
  uint32_t len;
  char *ret;

  if (!str)
    return NULL;

  len = (uint32_t)calc_len_for_utf8 (str);
  if ((ret = (char *)NPN_MemAlloc (len + 1)) == NULL)
    return NULL;

  len = (uint32_t)wcstoutf8 (str, ret, len);
  ret[len] = '\0';

  return ret;
}



/* decode */
#define UTF8_COMPUTE(Char, Mask, Len)					      \
  if (Char < 128)							      \
    {									      \
      Len = 1;								      \
      Mask = 0x7f;							      \
    }									      \
  else if ((Char & 0xe0) == 0xc0)					      \
    {									      \
      Len = 2;								      \
      Mask = 0x1f;							      \
    }									      \
  else if ((Char & 0xf0) == 0xe0)					      \
    {									      \
      Len = 3;								      \
      Mask = 0x0f;							      \
    }									      \
  else if ((Char & 0xf8) == 0xf0)					      \
    {									      \
      Len = 4;								      \
      Mask = 0x07;							      \
    }									      \
  else									      \
    Len = -1;


/*
 *  size      - size of buffer for output string in symbols (wchar_t)
 *  return    - length of output wchar_t string
 */
static size_t
utf8towcs (const char *ustr, wchar_t *wstr, size_t size)
{
  int i;
  int mask = 0;
  int len;
  unsigned char c;
  wchar_t wc;
  size_t count = 0;

  if (!ustr)
    return 0;

  while (count < size && (c = (unsigned char) *ustr))
    {
      UTF8_COMPUTE (c, mask, len);
      if (len == -1)
	return count;

      wc = c & mask;
      for (i = 1; i < len; i++)
	{
	  if ((ustr[i] & 0xC0) != 0x80)
	    return count;
	  wc <<= 6;
	  wc |= (ustr[i] & 0x3F);
	}
      *wstr = wc;
      ustr += len;
      wstr++;
      count++;
    }
  return count;
}


/*
 *  ulen      - length of input *ustr string in bytes
 *  size      - size of buffer ( *wstr string) in symbols
 *  converted - number of converted bytes from *ustr
 *
 *  Return    - length of output wcs string
 */
static size_t
utf8ntowcs (const char *ustr, wchar_t *wstr, size_t ulen, size_t size,
    u_short *converted)
{
  int i;
  int mask = 0;
  int len;
  unsigned char c;
  wchar_t wc;
  size_t count = 0;
  size_t _converted = 0;

  if (!ustr)
    return 0;

  while (_converted < ulen && count < size)
    {
      c = (unsigned char) *ustr;
      UTF8_COMPUTE (c, mask, len);
      if (len == -1 || _converted + len > ulen)
	{
	  if (converted)
	    *converted = (u_short) _converted;
	  return count;
	}

      wc = c & mask;
      for (i = 1; i < len; i++)
	{
	  if ((ustr[i] & 0xC0) != 0x80)
	    {
	      if (converted)
		*converted = (u_short) _converted;
	      return count;
	    }
	  wc <<= 6;
	  wc |= (ustr[i] & 0x3F);
	}
      *wstr = wc;
      ustr += len;
      wstr++;
      count++;
      _converted += len;
    }
  if (converted)
    *converted = (u_short) _converted;
  return count;
}

wchar_t *
strdup_U8toW (const char *str, int size)
{
  wchar_t *ret;
  uint32_t len;

  if (!str)
    return NULL;

  len = (uint32_t)utf8_len (str, size);
  if ((ret = (wchar_t *) NPN_MemAlloc ((len + 1) * sizeof (wchar_t))) == NULL)
    return NULL;

  if (size == -1) {
    len = (uint32_t)utf8towcs (str, ret, len);
    ret[len] = 0;
  } else {
    utf8ntowcs (str, ret, size, len, NULL);
    ret[len] = 0;
  }

  return ret;
}


int
vstrncmp (const char *s1, const char *s2, size_t n)
{
  int cmp;

  while (*s1 && n)
    {
      n--;
      if ((cmp = (*s1) - (*s2)) != 0)
	return cmp;
      s1++;
      s2++;
    }
  if (n)
    return (*s2) ? -1 : 0;
  return 0;
}


void AsyncParamFree(AsyncParam *param)
{
  NPN_ReleaseObject(param->pObj);
  NPN_MemFree(param);
}
