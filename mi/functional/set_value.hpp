/**
 * @file set_value.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_SET_VALUE_HPP__
#define __MI_FUNCTIONAL_SET_VALUE_HPP__ 1
#include <functional>

namespace mi
{
        template<typename T>
        class set_value : public std::unary_function<T, void>
        {
        private:
                const T _value;
        public:
                set_value ( const T v ) : _value( v ) {
                        return;
                }
                void operator () ( T& v ) {
                        v = this->_value;
                        return;
                }
        };
};
#endif //__MI_FUNCTIONAL_SET_VALUE_HPP__
