/**
 * @file minmax.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_MINMAX_HPP__
#define __MI_FUNCTIONAL_MINMAX_HPP__ 1
#include <functional>
#include <utility>

namespace mi
{
        template<typename T>
        class minmax : public std::unary_function<T, std::pair<T,T> >
        {
        private:
                std::pair<T, T>& _minmax;
                bool _isFirst;
        public:
                minmax ( std::pair<T,T>& minmax ) : _minmax( minmax ), _isFirst ( true ) {
                        return;
                }
                std::pair<T, T>& operator () ( const T& v ) {
                        if ( this->_isFirst ) {
                                this->_minmax = std::make_pair( v,v );
                                this->_isFirst = false;
                        } else {
                                if( v < this->_minmax.first  ) this->_minmax.first = v;
                                if( this->_minmax.second < v ) this->_minmax.second = v;
                        }
                        return this->_minmax;
                }
        };
};
#endif //__MI_FUNCTIONAL_MINMAX_HPP__
