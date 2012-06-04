#ifndef _NSODBCUTILS_H_
#define _NSODBCUTILS_H_

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined (UNIX)
#endif

#ifdef __APPLE__
#  include <iODBC/sql.h>
#  include <iODBC/sqlext.h>
#  include <iODBC/sqlucode.h>
#  include <iODBC/sqltypes.h>
#else
#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>
#endif

#include <wchar.h>
#include <string.h>

#include "mozilla/Mutex.h"
#include "nsMemory.h"
#include "nsCOMPtr.h"
#include "nsStringAPI.h"
#include "nsIVariant.h"


#define NUMCHAR(X)	(sizeof (X) / sizeof (SQLWCHAR))


/*
 *  Threading under windows
 */
#if defined (WIN32) && !defined (NO_THREADING)

# define OPL_SPINLOCK_DECLARE(M)	CRITICAL_SECTION M
# define OPL_SPINLOCK_INIT(M)		InitializeCriticalSection (&M)
# define OPL_SPINLOCK_DONE(M)		DeleteCriticalSection (&M)
# define OPL_SPINLOCK_LOCK(M)		EnterCriticalSection (&M)
# define OPL_SPINLOCK_UNLOCK(M)		LeaveCriticalSection (&M)

/*
 *  Threading with pthreads
 */
#elif defined (WITH_PTHREADS) && !defined (NO_THREADING)

# ifndef _REENTRANT
#  error Add -D_REENTRANT to your compiler flags
# endif

# include <pthread.h>

# define OPL_SPINLOCK_DECLARE(M)	pthread_mutex_t M
# ifndef OLD_PTHREADS
#  define OPL_SPINLOCK_INIT(M)		pthread_mutex_init (&M, NULL)
# else
#  define OPL_SPINLOCK_INIT(M)		pthread_mutex_init (&M, pthread_mutexattr_default)
# endif
# define OPL_SPINLOCK_DONE(M)		pthread_mutex_destroy (&M)
# define OPL_SPINLOCK_LOCK(M)		pthread_mutex_lock (&M)
# define OPL_SPINLOCK_UNLOCK(M)		pthread_mutex_unlock (&M)

#else
# error "NO THREADING"
#endif


class SpinLock {
private:
    OPL_SPINLOCK_DECLARE (m_spinLock);

public:
    SpinLock () { OPL_SPINLOCK_INIT (m_spinLock); }
    virtual ~SpinLock () { OPL_SPINLOCK_DONE (m_spinLock); }
    void Lock (void) { OPL_SPINLOCK_LOCK (m_spinLock); }
    void Unlock (void) { OPL_SPINLOCK_UNLOCK (m_spinLock); }
};



class NS_STACK_CLASS nsODBCAutoLock
{
public:
    nsODBCAutoLock(SpinLock& aLock) :
        mLock(&aLock)
    {
        NS_ASSERTION(mLock, "null mutex");
        mLocked = PR_TRUE;
        mLock->Lock();
    }
    
    ~nsODBCAutoLock(void) {
        if (mLocked)
          mLock->Unlock();
    }

    void lock() {
        PR_ASSERT(!mLocked);
        mLock->Lock();
        mLocked = PR_TRUE;
    }


    void unlock() {
        PR_ASSERT(mLocked);
        mLock->Unlock();
        mLocked = PR_FALSE;
    }
    
private:
    nsODBCAutoLock() {}
    nsODBCAutoLock(nsODBCAutoLock&) {}
    nsODBCAutoLock& operator=(nsODBCAutoLock&) { return *this; }
    static void* operator new(size_t) CPP_THROW_NEW { return nsnull; }
    static void operator delete(void*) {}

    PRBool mLocked;
    SpinLock* mLock;
};


class NS_STACK_CLASS nsODBCAutoUnlock
{
public:
    nsODBCAutoUnlock(SpinLock& aLock) :
        mLock(&aLock)
    {
        NS_ASSERTION(mLock, "null mutex");
        mLock->Unlock();
    }
    
    ~nsODBCAutoUnlock(void) {
        mLock->Lock();
    }

private:
    nsODBCAutoUnlock() {}
    nsODBCAutoUnlock(nsODBCAutoUnlock&) {}
    nsODBCAutoUnlock& operator=(nsODBCAutoUnlock&) { return *this; }
    static void* operator new(size_t) CPP_THROW_NEW { return nsnull; }
    static void operator delete(void*) {}

    SpinLock *mLock;
};


#if 0
class nsWString
{
public:
   nsWString(const nsAString & str)
   {
      PRBool    m_aTerm;
      const PRUnichar *m_Data = nsnull;
      m_wData = nsnull;
      m_Len = NS_StringGetData(str, &m_Data, &m_aTerm);

      m_wData = (SQLWCHAR*) calloc((m_Len + 1),sizeof(SQLWCHAR));
      for(PRInt32 i=0; i < m_Len; i++)
        m_wData[i] = (SQLWCHAR)m_Data[i];
      m_wData[m_Len] = 0;
   };

   nsWString(nsIVariant * val)
   {
      PRBool    m_aTerm;
      const PRUnichar *m_Data = nsnull;
      m_wData = nsnull;
      m_Len = -1;

      PRUint16 dtype;
      nsresult rv = val->GetDataType(&dtype);
      if (rv == NS_OK && dtype != nsIDataType::VTYPE_EMPTY) {
        nsString str;
        rv = val->GetAsAString(str);
        if (rv == NS_OK) {

          m_Len = NS_StringGetData(str, &m_Data, &m_aTerm);
          m_wData = (SQLWCHAR*) calloc((m_Len + 1),sizeof(SQLWCHAR));
          for(PRInt32 i=0; i < m_Len; i++)
            m_wData[i] = (SQLWCHAR)m_Data[i];
          m_wData[m_Len] = 0;

        }
      }
   };

   size_t Length()
   {
     return m_Len;
   };

   SQLWCHAR * Data()
   {
     return m_wData;
   };

   ~nsWString()
   {
     if (m_wData)
       free(m_wData);
   };

private:
  SQLWCHAR *m_wData;
  PRInt32  m_Len;
};
#endif

class nsWString
{
public:
   nsWString(const nsAString & str)
   {
      PRBool    m_aTerm;
      const PRUnichar *m_Data = nsnull;
      m_wData = nsnull;
      m_Len = NS_StringGetData(str, &m_Data, &m_aTerm);

      m_wData = (SQLWCHAR*) NS_Alloc((m_Len + 1) * sizeof(SQLWCHAR));
      for(PRInt32 i=0; i < m_Len; i++)
        m_wData[i] = (SQLWCHAR)m_Data[i];
      m_wData[m_Len] = 0;
   };

   nsWString(nsIVariant * val)
   {
      PRBool    m_aTerm;
      const PRUnichar *m_Data = nsnull;
      m_wData = nsnull;
      m_Len = -1;

      PRUint16 dtype;
      nsresult rv = val->GetDataType(&dtype);
      if (rv == NS_OK && dtype != nsIDataType::VTYPE_EMPTY) {
        nsString str;
        rv = val->GetAsAString(str);
        if (rv == NS_OK) {

          m_Len = NS_StringGetData(str, &m_Data, &m_aTerm);
          m_wData = (SQLWCHAR*) NS_Alloc((m_Len + 1) * sizeof(SQLWCHAR));
          for(PRInt32 i=0; i < m_Len; i++)
            m_wData[i] = (SQLWCHAR)m_Data[i];
          m_wData[m_Len] = 0;

        }
      }
   };

   size_t Length()
   {
     return m_Len;
   };

   SQLWCHAR * Data()
   {
     return m_wData;
   };

   ~nsWString()
   {
     if (m_wData)
       NS_Free(m_wData);
   };

private:
  SQLWCHAR *m_wData;
  PRInt32  m_Len;
};


class nsBinary
{
public:
   nsBinary(const PRUint8 *val, PRUint32 vsize)
   {
       m_Data = nsnull;
       m_Len = vsize;
       m_Data = (PRUint8*) NS_Alloc(m_Len);
       memcpy(m_Data, val, m_Len);
   };


   PRUint32 Length()
   {
     return m_Len;
   };

   PRUint8 * Data()
   {
     return m_Data;
   };

   ~nsBinary()
   {
     if (m_Data)
       NS_Free(m_Data);
   };

private:
  PRUint8 * m_Data;
  PRUint32  m_Len;
};



nsresult
NS_WStringToUTF16(const SQLWCHAR *sSource, nsAString &aDest);

nsresult
NS_AppendWStringToUTF16(const SQLWCHAR *sSource, nsAString &aDest, int len = -1);


void 
StoreError(SWORD fHandleType, SQLHANDLE handle, long &errorCode,
	nsString & errorMessage, nsString &errorState);


int vstrncmp (const char *s1, const char *s2, size_t n);


#endif /* _NSODBCUTILS_H_ */
