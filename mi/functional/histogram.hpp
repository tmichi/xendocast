/**
 * @file histogram.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_HISTOGRAM_HPP__
#define __MI_FUNCTIONAL_HISTOGRAM_HPP__ 1
#include <functional>
namespace mi
{
        template<typename T>
        class histogram : public std::unary_function<T, void>
        {
        private:
                T _minValue;
                T _maxValue;
                std::vector<int>& _bin;
        public:
                histogram ( const T& minValue, const T& maxValue, std::vector<int>& bin ) :
                        _minValue( minValue ), _maxValue( maxValue ), _bin( bin ) {
                        return;
                }
                void operator () ( const T& v ) const {
                        const int idx = this->get_index( v );
                        this->_bin[idx] += 1;
                        return;
                }
        private:
                inline int get_index( const T& v ) const {
                        if ( v <= this->_minValue ) return 0;
                        if ( this->_maxValue <= v ) return this->_bin.size() - 1;
                        return  static_cast<int>( ( v - _minValue ) *  this->_bin.size() / ( _maxValue - _minValue + 1 ) ) ;
                }
        };
};

#endif //__MI_FUNCTION_HISTOGRAM_HPP__


