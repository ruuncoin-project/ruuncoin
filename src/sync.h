#ifndef BITCOIN_SYNC_H
#define BITCOIN_SYNC_H

#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition_variable.hpp>
#include "threadsafety.h"

template <typename PARENT>
class LOCKABLE AnnotatedMixin : public PARENT
{
public:
    void lock() EXCLUSIVE_LOCK_FUNCTION()
    {
      PARENT::lock();
    }

    void unlock() UNLOCK_FUNCTION()
    {
      PARENT::unlock();
    }

    bool try_lock() EXCLUSIVE_TRYLOCK_FUNCTION(true)
    {
      return PARENT::try_lock();
    }
};

typedef AnnotatedMixin<boost::recursive_mutex> CCriticalSection;

typedef AnnotatedMixin<boost::mutex> CWaitableCriticalSection;

#ifdef DEBUG_LOCKORDER
void EnterCritical(const char* pszName, const char* pszFile, int nLine, void* cs, bool fTry = false);
void LeaveCritical();
#else
void static inline EnterCritical(const char* pszName, const char* pszFile, int nLine, void* cs, bool fTry = false) {}
void static inline LeaveCritical() {}
#endif

#ifdef DEBUG_LOCKCONTENTION
void PrintLockContention(const char* pszName, const char* pszFile, int nLine);
#endif

template<typename Mutex>
class CMutexLock
{
private:
    boost::unique_lock<Mutex> lock;

    void Enter(const char* pszName, const char* pszFile, int nLine)
    {
        EnterCritical(pszName, pszFile, nLine, (void*)(lock.mutex()));
#ifdef DEBUG_LOCKCONTENTION
        if (!lock.try_lock())
        {
            PrintLockContention(pszName, pszFile, nLine);
#endif
        lock.lock();
#ifdef DEBUG_LOCKCONTENTION
        }
#endif
    }

    bool TryEnter(const char* pszName, const char* pszFile, int nLine)
    {
        EnterCritical(pszName, pszFile, nLine, (void*)(lock.mutex()), true);
        lock.try_lock();
        if (!lock.owns_lock())
            LeaveCritical();
        return lock.owns_lock();
    }

public:
    CMutexLock(Mutex& mutexIn, const char* pszName, const char* pszFile, int nLine, bool fTry = false) : lock(mutexIn, boost::defer_lock)
    {
        if (fTry)
            TryEnter(pszName, pszFile, nLine);
        else
            Enter(pszName, pszFile, nLine);
    }

    ~CMutexLock()
    {
        if (lock.owns_lock())
            LeaveCritical();
    }

    operator bool()
    {
        return lock.owns_lock();
    }
};

typedef CMutexLock<CCriticalSection> CCriticalBlock;

#define LOCK(cs) CCriticalBlock criticalblock(cs, #cs, __FILE__, __LINE__)
#define LOCK2(cs1,cs2) CCriticalBlock criticalblock1(cs1, #cs1, __FILE__, __LINE__),criticalblock2(cs2, #cs2, __FILE__, __LINE__)
#define TRY_LOCK(cs,name) CCriticalBlock name(cs, #cs, __FILE__, __LINE__, true)

#define ENTER_CRITICAL_SECTION(cs) \
    { \
        EnterCritical(#cs, __FILE__, __LINE__, (void*)(&cs)); \
        (cs).lock(); \
    }

#define LEAVE_CRITICAL_SECTION(cs) \
    { \
        (cs).unlock(); \
        LeaveCritical(); \
    }

class CSemaphore
{
private:
    boost::condition_variable condition;
    boost::mutex mutex;
    int value;

public:
    CSemaphore(int init) : value(init) {}

    void wait() {
        boost::unique_lock<boost::mutex> lock(mutex);
        while (value < 1) {
            condition.wait(lock);
        }
        value--;
    }

    bool try_wait() {
        boost::unique_lock<boost::mutex> lock(mutex);
        if (value < 1)
            return false;
        value--;
        return true;
    }

    void post() {
        {
            boost::unique_lock<boost::mutex> lock(mutex);
            value++;
        }
        condition.notify_one();
    }
};

class CSemaphoreGrant
{
private:
    CSemaphore *sem;
    bool fHaveGrant;

public:
    void Acquire() {
        if (fHaveGrant)
            return;
        sem->wait();
        fHaveGrant = true;
    }

    void Release() {
        if (!fHaveGrant)
            return;
        sem->post();
        fHaveGrant = false;
    }

    bool TryAcquire() {
        if (!fHaveGrant && sem->try_wait())
            fHaveGrant = true;
        return fHaveGrant;
    }

    void MoveTo(CSemaphoreGrant &grant) {
        grant.Release();
        grant.sem = sem;
        grant.fHaveGrant = fHaveGrant;
        sem = NULL;
        fHaveGrant = false;
    }

    CSemaphoreGrant() : sem(NULL), fHaveGrant(false) {}

    CSemaphoreGrant(CSemaphore &sema, bool fTry = false) : sem(&sema), fHaveGrant(false) {
        if (fTry)
            TryAcquire();
        else
            Acquire();
    }

    ~CSemaphoreGrant() {
        Release();
    }

    operator bool() {
        return fHaveGrant;
    }
};
#endif

