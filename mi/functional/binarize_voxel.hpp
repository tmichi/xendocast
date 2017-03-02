/**
 * @file binarize_voxel.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */

#ifndef MI_FUNCTIONAL_BINARIZE_VOXEL_HPP
#define MI_FUNCTIONAL_BINARIZE_VOXEL_HPP 1
#include <functional>
#include <mi/VolumeData.hpp>

namespace mi
{
        template<typename T>
        class binarize_voxel: public std::unary_function<mi::Point3i, void>
        {
        private:
                const mi::VolumeData<T>& _dataFrom;
                mi::VolumeData<char>& _dataTo;
                const T _isoval;
                bool _negate;
        public:
                binarize_voxel ( const mi::VolumeData<T>& dataFrom, mi::VolumeData<char>& dataTo, const T& isoval , const bool negate )
                        : _dataFrom( dataFrom ), _dataTo( dataTo ) ,  _isoval( isoval ) , _negate( negate ) {
                        return;
                }

                void operator () ( const  mi::Point3i& p ) {
                        char val = this->_dataFrom.get( p ) < this->_isoval ? 0 : 1;
                        if ( this->_negate ) val = 1 - val;
                        this->_dataTo.set( p , val );
                        return;
                }
        };
};
#endif //MI_FUNCTIONAL_BINARIZE_VOXEL_HPP
