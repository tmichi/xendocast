#ifndef MI_VOLUME_DATA_UP_SAMPLER_HPP
#define MI_VOLUME_DATA_UP_SAMPLER_HPP 1
#include <iostream>
#include "VolumeData.hpp"

namespace mi
{
        template<typename T>
        class VolumeDataUpSampler
        {
        private:
                VolumeDataUpSampler( const VolumeDataUpSampler& that );
                void operator = ( const VolumeDataUpSampler& that );
        private:
                VolumeData<T> &_inData;
                VolumeData<T> &_outData;
        public:
                explicit VolumeDataUpSampler( VolumeData<T>& inData, VolumeData<T>& outData ) : _inData( inData ), _outData( outData ) {
                        return;
                }

                bool sample ( const int scale ) {
                        VolumeInfo& info     = this->_inData.getInfo();
                        const Point3i& size  = info.getSize();

                        VolumeInfo newInfo;
                        this->create_new_volume_info( scale, info, newInfo );
                        this->_outData.init( newInfo, true );

                        for( int z = 0 ; z < size.z() - 1; ++z ) {
                                for( int y = 0 ; y < size.y() - 1; ++y ) {
                                        for( int x = 0 ; x < size.x() - 1; ++x ) {
                                                const Point3i p( x,y,z );
                                                std::vector<T> values;
                                                for( int dz = 0 ; dz < 2 ; ++dz ) {
                                                        for( int dy = 0 ; dy < 2 ; ++dy ) {
                                                                for( int dx = 0 ; dx < 2 ; ++dx ) {
                                                                        values.push_back( this->_inData.get( x+dx, y+dy, z+dz ) );
                                                                }
                                                        }
                                                }

                                                for( int dz = 0 ; dz <= scale ; ++dz ) {
                                                        for( int dy = 0 ; dy <= scale ; ++dy ) {
                                                                for( int dx = 0 ; dx <= scale ; ++dx ) {
                                                                        Point3i dp( dx, dy, dz );
                                                                        if( dx == scale && x < size.x() - 2 ) continue;
                                                                        if( dy == scale && y < size.y() - 2 ) continue;
                                                                        if( dz == scale && z < size.z() - 2 ) continue;

                                                                        const double s = dx * 1.0 / scale;
                                                                        const double t = dy * 1.0 / scale;
                                                                        const double u = dz * 1.0 / scale;

                                                                        const double v0 = ( 1.0 - s ) * values[0] + s * values[1];
                                                                        const double v1 = ( 1.0 - s ) * values[2] + s * values[3];
                                                                        const double v2 = ( 1.0 - s ) * values[4] + s * values[5];
                                                                        const double v3 = ( 1.0 - s ) * values[6] + s * values[7];

                                                                        const double v4 = ( 1.0 - t ) * v0        + t * v1;
                                                                        const double v5 = ( 1.0 - t ) * v2        + t * v3;

                                                                        const double v6 = ( 1.0 - u ) * v4        + u * v5;

                                                                        const T result = static_cast<T>( v6 );

                                                                        Point3i np;
                                                                        np.x() = scale * p.x() + dp.x();
                                                                        np.y() = scale * p.y() + dp.y();
                                                                        np.z() = scale * p.z() + dp.z();
                                                                        this->_outData.set( np, result );
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                        return true;
                }

        private:
                void create_new_volume_info( const int scale , VolumeInfo& info, VolumeInfo& newInfo ) {
                        const Point3i& size  = info.getSize();
                        const Point3d& pitch = info.getPitch();

                        Point3i newSize;
                        newSize.x() = scale * size.x() + ( scale - 1 );
                        newSize.y() = scale * size.y() + ( scale - 1 );
                        newSize.z() = scale * size.z() + ( scale - 1 );

                        Point3d newPitch = pitch;
                        newPitch.x() *= 1.0 / scale;
                        newPitch.y() *= 1.0 / scale;
                        newPitch.z() *= 1.0 / scale;

                        newInfo.init( newSize, newPitch, info.getOrigin() );
                        return;
                }

        };
}
#endif//

