/**
 * @file Thread.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
//C4530
#ifndef MI_THREAD_HPP
#define MI_THREAD_HPP 1

#include <vector>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)// Win32 API
#define OS_WINDOWS 1
#include <windows.h>
#include <process.h>
#define ThreadResult unsigned __stdcall
#else // POSIX supporing system.
#include <pthread.h>
#define stdcall  ///< Ignorable macro.
#define ThreadResult void*  ///<Thread result.
#endif

namespace mi
{
        /**
         * @class Thread Thread.hpp "mi/Thread.hpp"
         * @brief Thread object.
         */
        class Thread
        {
        public:
#ifdef OS_WINDOWS
                typedef HANDLE Handle; ///< Thread handle.
                typedef CRITICAL_SECTION CriticalSectionHandle; ///< Critical section handle.
#else
                typedef pthread_t Handle; ///< Thread handle.
                typedef pthread_mutex_t CriticalSectionHandle;///< Critical section handle.
#endif
        private:
                Thread ( const Thread& that ) ;
                void operator = ( const Thread& that ) ;
        public:
                /**
                 * @brief Constructor.
                 */
                explicit Thread ( void ) : _sequence( 0 ) {
#ifdef OS_WINDOWS
                        InitializeCriticalSection ( &_cs );
#else //pthread
                        pthread_mutex_init ( &_cs, NULL );
#endif
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                ~Thread( void ) {
#ifdef OS_WINDOWS
                        DeleteCriticalSection ( &_cs );
#else //pthread
                        pthread_mutex_destroy ( &_cs );
#endif
                        return;
                }
                /**
                 * @brief Start critical section here.
                 * @sa Thread::endCriticalSection()
                 */
                void startCriticalSection( void ) {
#ifdef OS_WINDOWS
                        EnterCriticalSection ( &_cs );
#else //pthread
                        pthread_mutex_lock ( &_cs );
#endif
                        return;
                }
                /**
                 * @brief End critical section here.
                 * @sa Thread::startCriticalSection()
                 */
                void endCriticalSection( void ) {
#ifdef OS_WINDOWS
                        LeaveCriticalSection ( &_cs );
#else //pthread
                        pthread_mutex_unlock ( &_cs );
#endif
                        return;
                }
                /**
                 * @brief Create new thread.
                 * @param [in] func Function.
                 * @param [in] arglist Argument list.
                 * @return Thread ID.
                 */
#ifdef OS_WINDOWS
                int createThread ( unsigned int ( __stdcall * func ) ( void * ), void* arglist ) {
                        this->startCriticalSection();
                        Handle handle = ( Handle ) _beginthreadex ( NULL, 0, func, arglist, 0, NULL );
                        this->_handle.push_back ( handle );
                        const int threadId = this->getNumThread() - 1;
                        this->endCriticalSection();
                        return threadId;
                }
#else
                int createThread ( void * ( * func ) ( void * ), void* arglist ) {
                        this->startCriticalSection();
                        Handle handle;
                        pthread_create ( &handle, NULL, ( void * ( * ) ( void* ) ) func, arglist );
                        this->_handle.push_back ( handle );
                        const int threadId = this->getNumThread() - 1;
                        this->endCriticalSection();
                        return threadId;
                }
#endif
                /**
                 * @brief Wait all threads are terminated.
                 */
                void waitAll ( void ) {
                        for ( int i = 0 ; i  < this->getNumThread() ; i++ ) this->wait ( i );
                        return;
                }

                /**
                 * @brief Wait a thread is terminated.
                 * @param [in] id Thread ID.
                 */
                void wait ( const int id ) {
                        if ( id >= static_cast<int> ( this->getNumThread() ) ) return;
#ifdef OS_WINDOWS
                        WaitForSingleObject ( this->_handle[id],INFINITE );
#else //pthread
                        pthread_join ( this->_handle[id], NULL );
#endif
                        return;
                }

                /**
                 * @brief Close all threads.
                 */
                void closeAll ( void ) {
                        for ( int i = 0 ; i  < this->getNumThread() ; i++ ) this->close ( i );
                        return;
                }

                /**
                 * @brief Close a thread.
                 * @param [in] id Thread ID.
                 */
                bool close ( const int id ) const {
                        if ( id >=  this->getNumThread( ) ) return false;
#ifdef OS_WINDOWS
                        return ( CloseHandle ( this->_handle[id] ) != 0 );
#else //pthread
                        return true;
#endif
                }
                /**
                 * @brief Reset threads.
                 */
                void  reset ( void ) {
                        this->waitAll();
                        this->closeAll();
                        this->_handle.clear();
                        this->resetSequence();
                        return;
                }

                /**
                 * @brief Get the number of threads.
                 * @return Numrber of threads.
                 */
                inline int size ( void ) const {
                        return static_cast<int> ( this->_handle.size() );
                }

                /**
                 * @brief Get the number of threads.
                 * @return Numrber of threads.
                 */
                inline int getNumThread ( void ) const {
                        return this->size();
                }

                /**
                 * @brief Get a number of sequence.
                 * @return Number.
                 * @note This function is thread-safe or the number is always unique.
                 */
                int getSequence ( void ) {
                        this->startCriticalSection();
                        const int result = this->_sequence;
                        this->_sequence += 1;
                        this->endCriticalSection();
                        return result;
                }
                /**
                 * @brief Reset the sequence.
                 */
                void resetSequence ( void ) {
                        this->startCriticalSection();
                        this->_sequence = 0;
                        this->endCriticalSection();
                        return;
                }
        private:
                int         _sequence;
                std::vector<Handle>     _handle;
                CriticalSectionHandle	_cs;
        };
}
#endif // MI_THREAD_HPP
