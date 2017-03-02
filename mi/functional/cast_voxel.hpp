/**
 * @file cast_voxel.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_CAST_VOXEL_HPP__
#define __MI_FUNCTIONAL_CAST_VOXEL_HPP__ 1
#include <functional>
#include <mi/VolumeData.hpp>
namespace mi
{
        template<typename S, typename T>
        class cast_voxel: public std::unary_function<mi::Point3i, void>
        {
        private:
                const mi::VolumeData<S>& _dataFrom;
                mi::VolumeData<T>&       _dataTo;
        public:
                cast_voxel ( const mi::VolumeData<S>& dataFrom, mi::VolumeData<T>& dataTo ) : _dataFrom( dataFrom ), _dataTo( dataTo ) {
                        return;
                }

                void operator () ( const  mi::Point3i& p ) {
                        this->_dataTo.set( p, static_cast<T>( this->_dataTo.get( p ) ) );
                        return;
                }
        };
};
#endif //__MI_FUNCTIONAL_CAST_VOXEL_HPP__
