/**
 * @file VolumeDataClipper.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_VOLUME_DATA_CLIPPER_HPP
#define MI_VOLUME_DATA_CLIPPER_HPP  1
#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>

#include "VolumeData.hpp"
#include "FunctionObject.hpp"
namespace mi
{
        /**
         * @class VolumeDataClipper
         * @brief clip volume data.
         */
        template<typename T>
        class VolumeDataClipper
        {
        private:
                VolumeDataClipper ( const VolumeDataClipper& that ) ;
                void operator =   ( const VolumeDataClipper& that ) ;
        public:
                /**
                 * @brief Constructor.
                 */
                explicit VolumeDataClipper ( VolumeData<T>& data ) : _data( data ) {
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                ~VolumeDataClipper ( void ) {
                        return;
                }

                /**
                 * @brief clip volume data.
                 * @param [in] bmin Bounding box ( min ) .
                 * @param [in] bmax Bounding box ( max ) .
                 * @param [out] clippedImage Clipped result.
                 * @retval true Succeeded.
                 * @retval false Failed.
                 */
                int clip ( const  mi::Point3d& bmin, const mi::Point3d& bmax, mi::VolumeData<T>& clippedImage, const int nthread = 1 ) {
                        const mi::Point3i &minp = this->_data.getInfo().getPointInVoxel( bmin );
                        const mi::Point3i &maxp = this->_data.getInfo().getPointInVoxel( bmax );
                        return this->clip( minp, maxp, clippedImage );
                }
                /**
                 * @brief Clip volume data.
                 * @param [in] bmin Bounding box ( min ) .
                 * @param [in] bmax Bounding box ( max ) .
                 * @param [out] clippedImage Clipped result.
                 * @retval true Succeeded.
                 * @retval false Failed.
                 */
                bool clip ( const  mi::Point3i& bmin, const mi::Point3i& bmax, mi::VolumeData<T>& clippedImage ) {
                        const mi::VolumeInfo& info = this->_data.getInfo();
                        if ( !info.isValid( bmin ) || !info.isValid( bmax ) ) {
                                std::cerr<<"error. bounding box is out-of-range."<<std::endl;
                                return false;
                        }
                        if( bmax.x() < bmin.x() || bmax.y() < bmin.y() || bmax.z() < bmin.z() ) {
                                std::cerr<<"bmin and bmax are invalid"<<std::endl;
                                return false;
                        }
                        const mi::Point3i size   = bmax - bmin + mi::Point3i( 1,1,1 );
                        const mi::Point3d &pitch  = this->_data.getInfo().getPitch();
                        const mi::Point3d &origin = this->_data.getInfo().getPointInSpace( bmin );
                        mi::VolumeInfo newInfo( size, pitch, origin );
                        std::for_each( newInfo.begin(), newInfo.end(), copy_voxel<T>( this->_data, clippedImage, bmin ) );
                        return true;
                }
        private:
                VolumeData<T>& _data; ///< Original image.
        };
}
#endif
