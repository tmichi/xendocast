/**
 * @file ParallelFor.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_PARALLEL_FOR_HPP
#define MI_PARALLEL_FOR_HPP 1
#include <algorithm>
#include "Thread.hpp"
namespace mi
{
        /**
         * @brief Parallel implementation of std::foreach().
         * @param [in] begin Begin iterator.
         * @param [in] end End iterator.
         * @param [in] fn Functor.
         * @param [in] grainSize Grain size.
         */
        template <class Iterator, class Function>
        void parallel_for_each( const Iterator begin, const Iterator end, const Function fn, const int grainSize = 1000 )
        {
                class ParallelFor
                {
                private:
                        class packed_data
                        {
                        public:
                                Iterator begin;
                                Iterator end;
                                Function fn;
                        public:
                                explicit packed_data( Iterator b, Iterator e,  Function f ) : begin( b ), end( e ), fn( f ) {
                                        return;
                                }
                        };
                public:
                        ParallelFor ( const Iterator begin, const Iterator end, const Function fn, const int grainSize ) {
                                Thread thread;
                                Iterator start = begin;
                                Iterator iter  = begin;

                                while ( start != end ) {
                                        for( int i = 0 ; i < grainSize ; ++i ) {
                                                if ( iter == end ) break;
                                                ++iter; // += operator cannot be used .
                                        }
                                        packed_data* p = new packed_data( start, iter, Function( fn ) ); //deleted in child_thread() ;
                                        thread.createThread( ParallelFor::child_thread, p );
                                        start = iter;

                                }

                                thread.waitAll();
                                thread.closeAll();
                                return;
                        }
                private:
                        static ThreadResult child_thread( void* arg ) {
                                packed_data* p = reinterpret_cast<packed_data*>( arg );
                                std::for_each( p->begin, p->end, p->fn );
                                delete p;
                                return 0;
                        }
                };
                ParallelFor( begin, end, fn, grainSize );
                return;
        };
};
#endif //MI_PARALLEL_FOR_HPP
