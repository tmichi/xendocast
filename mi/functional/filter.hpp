/**
 * @file filter.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_FILTER_HPP__
#define __MI_FUNCTIONAL_FILTER_HPP__ 1
#include <functional>

namespace mi
{
        template<typename T>
        class filter : public std::unary_function<T, T>
        {
        private:
                T _value;
        public:
                filter ( const T& value ) : _value ( value ) {
                        return;
                }

                T operator () ( const T& v ) const {
                        return ( v == this->_value ) ? v : T();
                }
        };
};

#endif //__MI_FUNCTIONAL_FILTER_HPP__
