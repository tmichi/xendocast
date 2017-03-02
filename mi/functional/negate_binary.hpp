/**
 * @file negate_binary.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef MI_FUNCTIONAL_NEGATE_BINARY_HPP
#define MI_FUNCTIONAL_NEGATE_BINARY_HPP 1
#include <functional>
namespace mi
{
        template<typename T>
        class negate_binary : public std::unary_function<void, T>
        {
        private:
                const VolumeData<T>& _inData;
                VolumeData<T>& _outData;
        public:
                negate_binary ( const VolumeData<T>& inData, VolumeData<T>& outData ) : _inData( inData ), _outData( outData ) {
                        return;
                }

                void operator () ( const Point3i& p ) const {
                        this->_outData.set( p, 1 - this->_inData.get( p ) );
                }
        };
};

#endif //__MI_FUNCTIONAL_NEGATE_BINARY_HPP__
