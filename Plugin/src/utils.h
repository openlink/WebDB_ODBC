#ifndef _UTILS_H
#define _UTILS_H


/*
 *  Max length of a UTF-8 encoded character sequence
 */
#define UTF8_MAX_CHAR_LEN 4
#define NUMCHAR(X)	(sizeof (X) / sizeof (SQLWCHAR))

int vstrncmp (const char *s1, const char *s2, size_t n);

/*
 *  Function Prototypes
 */
char *strdup_WtoU8 (const wchar_t * str);

wchar_t *strdup_U8toW (const char *str, int size=-1);


class nsWString
{
public:
   nsWString(const char *str)
   {
      m_Data = NULL;
      if (str != NULL) {
        m_Data = strdup_U8toW(str);
        m_Len = (SQLUINTEGER)wcslen(m_Data);
      }
   };


   nsWString(NPString *val)
   {
      char *str = (char *)val->utf8characters;
      int len = val->utf8length;
      m_Data = NULL;
      if (str != NULL) {
        m_Data = strdup_U8toW(str, len);
        m_Len = (SQLUINTEGER) wcslen(m_Data);
      }
   };

   SQLULEN Length()
   {
     return m_Len;
   };

   SQLWCHAR * Data()
   {
     return (SQLWCHAR *) m_Data;
   };

   ~nsWString()
   {
     if (m_Data)
       NPN_MemFree(m_Data);
   };

private:
  SQLWCHAR *m_Data;
  SQLULEN  m_Len;
};





#define STRING_TO_NPVARIANT(_val, _v)                                        \
NP_BEGIN_MACRO                                                               \
   const char *__val = _val;                                                 \
    (_v).type = NPVariantType_String;                                        \
    (_v).value.stringValue.utf8characters = __val;                           \
    (_v).value.stringValue.utf8length =strlen(__val);                        \
NP_END_MACRO


#endif /* _UTILS_H */

