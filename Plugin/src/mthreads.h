#ifndef _MTHREADS_H
#define _MTHREADS_H

/*
 *  Threading under windows
 */
#if defined (WIN32) && !defined (NO_THREADING)

# define OPL_THREADING

# define OPL_THREAD_IDENT		GetCurrentThreadId ()

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

# define OPL_THREADING

# define OPL_SPINLOCK_DECLARE(M)	pthread_mutex_t M
# ifndef OLD_PTHREADS
#  define OPL_THREAD_IDENT		((long) (pthread_self ()))
#  define OPL_SPINLOCK_INIT(M)		pthread_mutex_init (&M, NULL)
# else
#  undef OPL_THREAD_IDENT
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
    void Enter (void) { OPL_SPINLOCK_LOCK (m_spinLock); }
    void Leave (void) { OPL_SPINLOCK_UNLOCK (m_spinLock); }
};

class AutoLock 
{
private:
  SpinLock *m_spinLock;

public:
  AutoLock (SpinLock& rSpinLock)
	{ m_spinLock = &rSpinLock; m_spinLock->Enter(); }
  ~AutoLock () 
	{ m_spinLock->Leave(); }

};


typedef struct _AsyncParam
{
  NPP        npp;
  NPObject   *pObj;
  NPIdentifier  mc_run_id;
} AsyncParam;

void AsyncParamFree(AsyncParam *param);

struct AsyncFunctionCall
{
  AsyncFunctionCall(NPP _npp, void (*f)(void *), void *ud)
  	: npp(_npp), func(f), userData(ud) {}

  NPP  npp;
  void (*func)(void *);
  void *userData;
};


#endif /* _UTILS_H */

