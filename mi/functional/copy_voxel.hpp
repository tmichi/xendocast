/**
 * @file copy_voxel.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_COPY_VOXEL_HPP__
#define __MI_FUNCTIONAL_COPY_VOXEL_HPP__ 1
#include <functional>
#include <mi/VolumeData.hpp>
namespace mi
{
        template<typename T>
        class copy_voxel: public std::unary_function<mi::Point3i, void>
        {
        private:
                const mi::VolumeData<T>& _dataFrom;
                mi::VolumeData<T>& _dataTo;
                const mi::Point3i _offset;
        public:
                copy_voxel ( const mi::VolumeData<T>& dataFrom, mi::VolumeData<T>& dataTo, const mi::Point3i& offset )
                        : _dataFrom( dataFrom ), _dataTo( dataTo ) ,  _offset( offset ) {
                        return;
                }
                void operator () ( const  mi::Point3i& p ) {
                        this->_dataTo.set( p , this->_dataFrom.get( p + _offset ) );
                        return;
                }
        };
};

#endif //__MI_FUNCTIONAL_OBJECT_HPP__
