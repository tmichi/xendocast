/**
 * @file cast.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_CAST_HPP__
#define __MI_FUNCTIONAL_CAST_HPP__ 1
#include <functional>

namespace mi
{
        template<typename S, typename T>
        class cast : public std::unary_function<S, T>
        {
        public:
                T operator () ( const S& v ) const {
                        return static_cast<T>( v );
                }
        };
};

#endif //__MI_FUNCTIONAL_CAST_HPP__
