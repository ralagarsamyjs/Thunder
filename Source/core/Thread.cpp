 /*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2020 Metrological
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "Thread.h"
#include "Proxy.h"
#include "Serialization.h"
#include "Trace.h"
#include <limits.h>

#ifdef __WINDOWS__
#include <process.h>
#endif

//-----------------------------------------------------------------------------------------------
// CLASS: Thread
//-----------------------------------------------------------------------------------------------

namespace WPEFramework {
namespace Core {

    //Definitions of static members
    uint32_t Thread::_defaultStackSize = 0;
    #ifdef __CORE_EXCEPTION_CATCHING__
    Thread::IExceptionCallback* Thread::_exceptionHandler = nullptr;
    #endif


    Thread::Thread(const uint32_t stackSize, const TCHAR* threadName)
        : m_enumState(BLOCKED)
        , m_syncAdmin()
        , m_sigExit(false, true)

#ifdef __WINDOWS__
        , m_enumSuspendedState(BLOCKED)
        , m_hThreadInstance(nullptr)
        , m_ThreadId(0)
#else
        , m_hThreadInstance()
        , m_ThreadId(0)
#endif
    {
        TRACE_L5("Constructor Thread <%p>", (this));
        printf("WPEFramework::Core::Thread::Thread()->PID<%d><%d> Constructor Thread\n", getpid(), gettid());

// Create a worker that can do actions in parallel
#ifdef __WINDOWS__

        m_hThreadInstance = ::CreateThread(nullptr,
            0,
            (LPTHREAD_START_ROUTINE)Thread::StartThread,
            (LPVOID)this,
            0,
            &m_ThreadId);

        // If there is no thread, the "new" thread can also not free the destructor,
        // then it is up to us.
        if (m_hThreadInstance == nullptr)
#endif

#ifdef __POSIX__
            int err;

        pthread_attr_t attr;

        err = pthread_attr_init(&attr);
        ASSERT(err == 0);

        if ((err == 0) && (stackSize != 0)) {
            size_t new_size = (stackSize < PTHREAD_STACK_MIN) ? PTHREAD_STACK_MIN : stackSize;
            err = pthread_attr_setstacksize(&attr, new_size);
            ASSERT(err == 0);
        }

        // If there is no thread, the "new" thread can also not free the destructor,
        // then it is up to us.
        if ((err != 0) || (pthread_create(&m_hThreadInstance, &attr, (void* (*)(void*))Thread::StartThread, this) != 0))
#endif
        {
            // Creation failed, O.K. We will signal the inactive state our selves.
            m_enumState = FAILED;
            m_sigExit.SetEvent();
        }

#ifdef __POSIX__
        err = pthread_attr_destroy(&attr);
        ASSERT(err == 0);

        m_ThreadId = m_hThreadInstance;
#endif

        if (threadName != nullptr) {
            std::string convertedName;
            Core::ToString(threadName, convertedName);

            ThreadName(convertedName.c_str());
            printf("WPEFramework::Core::Thread::Thread()->PID<%d><%d>m_ThreadId<%ld>ThreadName<%s>\n", getpid(), gettid(), m_ThreadId, convertedName.c_str());
        }
        printf("WPEFramework::Core::Thread::Thread()->PID<%d><%d>m_ThreadId<%ld>\n", getpid(), gettid(), m_ThreadId);

    }
    Thread::~Thread()
    {
        TRACE_L5("Destructor Thread <%p>", (this));
        printf("WPEFramework::Core::Thread::~Thread()->PID<%d><%d> calling Terminate()\n", getpid(), gettid());

        Terminate();
    }


    void Thread::Signal(const int signal) const
    {
#ifdef __LINUX__
       printf("WPEFramework::Core::Thread::Signal()->PID<%d><%d>m_ThreadId<%ld>signal<%d>\n", getpid(), gettid(), m_ThreadId, signal);
       ::pthread_kill(m_hThreadInstance, signal);
#endif
    }

    ::ThreadId Thread::ThreadId()
    {
#ifdef __WINDOWS__
PUSH_WARNING(DISABLE_WARNING_CONVERSION_TO_GREATERSIZE)
        return (::GetCurrentThreadId());
POP_WARNING()
#else
        return static_cast<::ThreadId>(pthread_self());
#endif
    }

#ifdef __WINDOWS__
    void Thread::StartThread(Thread* cClassPointer)
#endif

#ifdef __POSIX__
        void* Thread::StartThread(Thread* cClassPointer)
#endif
    {
#ifdef __POSIX__
        // It is the responsibility of the main app to make sure all threads created are stopped properly.
        // No jumping and bailing out without a proper closure !!!!
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGTERM);
        sigaddset(&mask, SIGPIPE);
        pthread_sigmask(SIG_BLOCK, &mask, nullptr);
#endif

        StateTrigger<thread_state>& stateObject = cClassPointer->m_enumState;
        printf("WPEFramework::Core::Thread::StartThread()->PID<%d><%d> calling stateObject.WaitState() \n", getpid(), gettid());
        stateObject.WaitState(INITIALIZED | DEACTIVATE | RUNNING | STOPPED | STOPPING, Core::infinite);

        if (((stateObject & (STOPPED | STOPPING)) == 0) && (cClassPointer->Initialize() == Core::ERROR_NONE)) {
            CriticalSection& adminLock = cClassPointer->m_syncAdmin;

            printf("WPEFramework::Core::Thread::StartThread()->PID<%d><%d> calling adminLock.Lock() \n", getpid(), gettid());
            // O.K. befor using the state, lock it.
            adminLock.Lock();

            if ( (stateObject == INITIALIZED) || (stateObject == DEACTIVATE) ) {
                printf("WPEFramework::Core::Thread::StartThread((stateObject == INITIALIZED) || (stateObject == DEACTIVATE) )->PID<%d><%d> calling cClassPointer->State(BLOCKED)\n", getpid(), gettid());
                cClassPointer->State(BLOCKED);
            }

            // Do the work that is requested.
            while ((stateObject & (STOPPED | STOPPING)) == 0) {
                unsigned int delayed = Core::infinite;

                if (stateObject == RUNNING) {
                    printf("WPEFramework::Core::Thread::StartThread(stateObject == RUNNING)->PID<%d><%d><%ld> calling adminLock.Unlock()\n", getpid(), gettid(), cClassPointer->m_hThreadInstance);
                    // O.K. befor using the state, lock it.
                    adminLock.Unlock();

                    #ifdef __CORE_EXCEPTION_CATCHING__
                    try {
                        delayed = cClassPointer->Worker();
                    }
                    catch(const std::exception& type) {
                        if(_exceptionHandler != nullptr){
                            _exceptionHandler->Exception(type.what());
                        }
                    }
                    catch(...) {
                        if(_exceptionHandler != nullptr){
                            _exceptionHandler->Exception(_T("Unknown"));
                        }
                    }
                    #else
                        printf("WPEFramework::Core::Thread::StartThread(stateObject == RUNNING)->PID<%d><%d> calling cClassPointer->Worker()\n", getpid(), gettid());
                        delayed = cClassPointer->Worker();
                    #endif
                    printf("WPEFramework::Core::Thread::StartThread(stateObject == RUNNING)->PID<%d><%d> calling adminLock.Lock()\n", getpid(), gettid());
                    // Change the state, we are done with it.
                    adminLock.Lock();
                }

                if (stateObject == DEACTIVATE) {
                    printf("WPEFramework::Core::Thread::StartThread(stateObject == DEACTIVATE)->PID<%d><%d> calling cClassPointer->State(BLOCKED)\n", getpid(), gettid());
                    cClassPointer->State(BLOCKED);
                }
                printf("WPEFramework::Core::Thread::StartThread()->PID<%d><%d> calling adminLock.Unlock()\n", getpid(), gettid());
                // O.K. before we wait for a new state unlock the current stuff.
                adminLock.Unlock();

                printf("WPEFramework::Core::Thread::StartThread()->PID<%d><%d>delayed<0x%8x> calling stateObject.WaitState(RUNNING | STOPPED | STOPPING, delayed)\n", getpid(), gettid(), delayed);
                // Wait till we reach a runnable state
                stateObject.WaitState(RUNNING | STOPPED | STOPPING, delayed);
                printf("WPEFramework::Core::Thread::StartThread()->PID<%d><%d>delayed<0x%8x> calling adminLock.Lock()\n", getpid(), gettid(), delayed);
                // Change the state, we are done with it.
                adminLock.Lock();

                // Check in which state we reached the criteria for the WaitState !!
                if (stateObject == BLOCKED) {
                    printf("WPEFramework::Core::Thread::StartThread(stateObject == BLOCKED))->PID<%d><%d>delayed<0x%8x> calling cClassPointer->State(RUNNING)\n", getpid(), gettid(), delayed);
                    cClassPointer->State(RUNNING);
                }
            }
            printf("WPEFramework::Core::Thread::StartThread()->PID<%d><%d>delayed<0x%8x> calling adminLock.Unlock()\n", getpid(), gettid());
            // O.K. befor using the state, lock it.
            adminLock.Unlock();
        }
        printf("WPEFramework::Core::Thread::StartThread()->PID<%d><%d> calling  cClassPointer->State(STOPPED)\n", getpid(), gettid());
        cClassPointer->State(STOPPED);
        printf("WPEFramework::Core::Thread::StartThread()->PID<%d><%d> calling  cClassPointer->m_sigExit.SetEvent()\n", getpid(), gettid());
        // Report that the worker is done by releasing the Signal sync mechanism.
        cClassPointer->m_sigExit.SetEvent();

#ifndef __WINDOWS__
        printf("WPEFramework::Core::Thread::StartThread()->PID<%d><%d> return \n", getpid(), gettid());
        return (nullptr);
#else 
        ::ExitThread(0);
#endif
    }

    uint32_t Thread::Initialize()
    {
        printf("WPEFramework::Core::Thread::Initialize()->PID<%d><%d> return \n", getpid(), gettid());
        return (Core::ERROR_NONE);
    }

    void Thread::Terminate()
    {
        // Make sure the thread knows that it should stop.
        m_syncAdmin.Lock();
        printf("WPEFramework::Core::Thread::Terminate()->PID<%d><%d> calling State(STOPPING)\n", getpid(), gettid());
        State(STOPPING);

        m_syncAdmin.Unlock();

        // We have to wait till the thread is completely stopped. If we
        // would continue here, the memory allocated for this instance of
        // this class would be freed. If our worker thread would still
        // be running, it will still reference to the data stored in the
        // allocated memory for the instance of this class. In the mean
        // time this data might have been taken by another thread/procces
        // and thay might have changed the data with all consequences, so
        // let's wait until we are triggered that the thread is dead.
        // We do not want to be busy waiting so do it via a synchronisation
        // mechanism, e.g. the semaphore created during construction
        // time.

        m_sigExit.Lock(Core::infinite);

#ifdef __POSIX__
        if (!IsFailed()) {
            void* l_Dummy;
            printf("WPEFramework::Core::Thread::Terminate()->PID<%d><%d> calling ::pthread_join(m_hThreadInstance, &l_Dummy)\n", getpid(), gettid());
            ::pthread_join(m_hThreadInstance, &l_Dummy);
        }
#endif
        printf("WPEFramework::Core::Thread::Terminate()->PID<%d><%d> return \n", getpid(), gettid());
    }

    void Thread::Suspend()
    {
#ifdef __POSIX__
        printf("WPEFramework::Core::Thread::Suspend()->PID<%d><%d> calling Block() \n", getpid(), gettid());
        Block();
#else
        State(SUSPENDED);
#endif
    }

    void Thread::Init()
    {
        m_syncAdmin.Lock();
        printf("WPEFramework::Core::Thread::Init()->PID<%d><%d> calling State(INITIALIZED) \n", getpid(), gettid());
        State(INITIALIZED);

        m_syncAdmin.Unlock();
    }

    void Thread::Stop()
    {
        m_syncAdmin.Lock();
        printf("WPEFramework::Core::Thread::Stop()->PID<%d><%d> calling State(STOPPING) \n", getpid(), gettid());
        State(STOPPING);

        m_syncAdmin.Unlock();
    }

    void Thread::Block()
    {
        m_syncAdmin.Lock();
        printf("WPEFramework::Core::Thread::Block()->PID<%d><%d><%ld> calling State(DEACTIVATE) \n", getpid(), gettid(), m_hThreadInstance);
        State(DEACTIVATE);

        m_syncAdmin.Unlock();
    }

    void Thread::Run()
    {
        m_syncAdmin.Lock();
        printf("WPEFramework::Core::Thread::Run()->PID<%d><%d> calling State(RUNNING) \n", getpid(), gettid());
        State(RUNNING);

        m_syncAdmin.Unlock();
    }

    int Thread::PriorityMin() const
    {
#ifdef __POSIX__
        return (sched_get_priority_min(SCHED_OTHER));
#else
        return (0);
#endif
    }

    int Thread::PriorityMax() const
    {
#ifdef __POSIX__
        return (sched_get_priority_max(SCHED_OTHER));
#else
        return (255);
#endif
    }

    bool Thread::Priority(int priority)
    {
        bool result = false;

#ifdef __POSIX__
        struct sched_param p;
        p.sched_priority = priority;
        pthread_setschedparam(m_hThreadInstance, SCHED_OTHER, &p);
        result = (0 == pthread_setschedparam(m_hThreadInstance, SCHED_OTHER, &p));
#endif

        return (result);
    }

    bool Thread::Wait(const unsigned int enumState, unsigned int nTime) const
    {
        return (IsFailed() ? false : m_enumState.WaitState(enumState, nTime));
    }

    Thread::thread_state Thread::State() const
    {
        // Return the current status of the worker thread.
        return (m_enumState);
    }

    bool
    Thread::State(Thread::thread_state enumNewState)
    {
        bool blOK = false;

        switch (m_enumState) {
        case INITIALIZED:
            blOK = ((enumNewState == RUNNING) || (enumNewState == BLOCKED) || (enumNewState == STOPPED) || (enumNewState == STOPPING));
            break;
        case SUSPENDED:
            blOK = ((enumNewState == RUNNING) || (enumNewState == STOPPED) || (enumNewState == STOPPING));
            break;
        case RUNNING:
            blOK = ((enumNewState == SUSPENDED) || (enumNewState == INITIALIZED) || (enumNewState == BLOCKED) || (enumNewState == DEACTIVATE) || (enumNewState == STOPPING) || (enumNewState == STOPPED));
            break;
        case DEACTIVATE:
            blOK = ((enumNewState == SUSPENDED) || (enumNewState == BLOCKED) || (enumNewState == RUNNING) || (enumNewState == STOPPING) || (enumNewState == STOPPED));
            break;
        case BLOCKED:
            blOK = ((enumNewState == SUSPENDED) || (enumNewState == INITIALIZED) || (enumNewState == RUNNING) || (enumNewState == STOPPING) || (enumNewState == STOPPED));
            break;
        case STOPPING:
            blOK = ((enumNewState == STOPPING) || (enumNewState == STOPPED));
            break;
        case STOPPED: // The STOPPED state is the end,
            // No changes possible anymore.
            blOK = (enumNewState == STOPPED);
            break;
        case FAILED: // There's no thread, so nothing we can do!
            blOK = false;
            break;
        }

        if (blOK) {
#ifndef __POSIX__
            if (enumNewState == SUSPENDED) {
                m_enumSuspendedState = m_enumState;
                m_enumState.SetState(SUSPENDED);

                // O.K. Suspend this thread.
                ::SuspendThread(m_hThreadInstance);
            } else if (m_enumState == SUSPENDED) {
                m_enumState.SetState(m_enumSuspendedState);

                // Done in the suspended state, resume.
                ::ResumeThread(m_hThreadInstance);
            } else
#endif
            {
                m_enumState.SetState(enumNewState);
            }
        }

        return (blOK);
    }

    void Thread::ThreadName(const char* threadName VARIABLE_IS_NOT_USED)
    {
#ifdef __WINDOWS__
#ifdef __DEBUG__

#pragma pack(push, 8)

        struct tagTHREADNAME_INFO {
            DWORD dwType; // Must be 0x1000.
            LPCSTR szName; // Pointer to name (in user addr space).
            DWORD dwThreadID; // Thread ID (-1=caller thread).
            DWORD dwFlags; // Reserved for future use, must be zero.
        } info;

#pragma pack(pop)

        info.dwType = 0x1000;
        info.szName = threadName;
        info.dwThreadID = m_ThreadId;
        info.dwFlags = 0;

        __try {
            RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
        }
#endif // __DEBUG__
#else
        int rc = pthread_setname_np(m_hThreadInstance, threadName);
        if (rc == ERANGE) {
            // name too long - max 16 chars allowed
            char truncName[16];
            strncpy(truncName, threadName, sizeof(truncName));
            truncName[15] = '\0';
            pthread_setname_np(m_hThreadInstance, truncName);
        }
#endif // __WINDOWS__
    }

#ifdef __DEBUG__
    int Thread::GetCallstack(void** buffer, int size)
    {
#if defined(THUNDER_BACKTRACE)
        return GetCallStack(m_hThreadInstance, buffer, size);
#else
        return(0);
#endif
    }
#endif // __DEBUG
}
} // namespace Core
