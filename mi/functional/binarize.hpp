/**
 * @file binarize.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_BINARIZE_HPP__
#define __MI_FUNCTIONAL_BINARIZE_HPP__ 1
#include <functional>
namespace mi
{
        template<typename S, typename T = S>
        class binarize : public std::unary_function<S, T>
        {
        private:
                const S _threshold;
                const T _onValue;
                const T _offValue;
        public:
                binarize ( const S threshold = S( 1 ) , const T onValue = 1, const T offValue = 0 ) : _threshold ( threshold ), _onValue ( onValue ), _offValue( offValue ) {
                        return;
                }

                T operator () ( const S& v ) const {
                        return ( v < this->_threshold ) ? this->_offValue : this->_onValue;
                }
        };
};
#endif //__MI_FUNCTIONAL_BINARIZE_HPP__
