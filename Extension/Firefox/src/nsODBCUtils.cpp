
#include "nsODBCUtils.h"

nsresult
NS_WStringToUTF16(const SQLWCHAR *sSource, nsAString &aDest)
{
  size_t len;

  if (sizeof(SQLWCHAR) == sizeof(PRUnichar)) {
    aDest.Assign((PRUnichar *)sSource); 
    return NS_OK;
  } else {
    len = wcslen(sSource);
    aDest.Truncate();
    for(PRUint32 i = 0; i < len; i++)
	aDest.Append((PRUnichar)sSource[i]);
    return NS_OK;
  }
}


nsresult
NS_AppendWStringToUTF16(const SQLWCHAR *sSource, nsAString &aDest, int alen)
{
  if (sizeof(SQLWCHAR) == sizeof(PRUnichar)) {
    aDest.Append((PRUnichar *)sSource); 
    return NS_OK;
  } else {
    PRUint32 len = (alen == -1 ? (PRUint32)wcslen(sSource) : alen);
    PRUint32 dlen = aDest.Length();
    aDest.SetLength(dlen + len);

    PRUnichar *cur;
    NS_StringGetMutableData(aDest, PR_UINT32_MAX, &cur);
    cur += dlen;

    for(PRUint32 i = 0; i < len; i++)
        *cur++ = (PRUnichar)sSource[i];
    return NS_OK;
  }
}


void StoreError(SWORD fHandleType, SQLHANDLE handle, long &errorCode,
	nsString & errorMessage, nsString &errorState)
{	
    SQLWCHAR szErrState[SQL_SQLSTATE_SIZE+1];    // SQL Error State string
    SQLINTEGER dwErrCode;			 // Native Error code
    SQLWCHAR szErrText[SQL_MAX_MESSAGE_LENGTH+1];// SQL Error Text string
    SWORD    wErrMsgLen;			 // Error message length
	
    SQLWCHAR szBuffer[600];	                 //SQL_SQLSTATE_SIZE+SQL_MAX_MESSAGE_LENGTH+1];
    szBuffer[0] = '\0';		
	
    SQLRETURN  nErrResult;  // Return Code from SQLGetDiagRec
    SWORD sMsgNum = 1;	    // no of records

    errorMessage.Assign(NS_LITERAL_STRING(""));

    nErrResult = SQLGetDiagRecW(fHandleType, handle, sMsgNum,
		szErrState, &dwErrCode, szErrText,
		SQL_MAX_MESSAGE_LENGTH-1, &wErrMsgLen);
    if (nErrResult != SQL_NO_DATA) 
    {	
	if(nErrResult == SQL_ERROR || nErrResult == SQL_INVALID_HANDLE){
	    errorMessage.Assign(NS_LITERAL_STRING("\nError in executing SQLGetDiagRecW function"));			
	} else {
        			
#ifdef XP_WIN
          _snwprintf((wchar_t *)szBuffer, sizeof(szBuffer)/sizeof(wchar_t), L"\nSQL Error State  : %s\nNative Error Code: %lX\nODBC Error       : %s",(LPSTR) szErrState, dwErrCode, (LPWSTR)szErrText);				
#else
          swprintf((wchar_t *)szBuffer, sizeof(szBuffer)/sizeof(wchar_t), L"\nSQL Error State  : %ls\nNative Error Code: %lX\nODBC Error       : %ls",(LPSTR) szErrState, dwErrCode, (LPWSTR)szErrText);				
#endif
          NS_AppendWStringToUTF16(szBuffer, errorMessage);
          NS_WStringToUTF16(szErrState, errorState);
	  errorCode = (dwErrCode?dwErrCode:1);
        }
    }	
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

