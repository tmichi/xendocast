/**
 * @file erode.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_ERODE_HPP__
#define __MI_FUNCTIONAL_ERODE_HPP__ 1
#include <functional>
#include <mi/VolumeData.hpp>
namespace mi
{
        class erode
        {
        private:
                VolumeData<char>& _inData;
                VolumeData<char>& _outData;
                const double _radius;
                Point3d _pitch; // (x*x, y*y, z*z)
        public:
                erode ( VolumeData<char>& inData, VolumeData<char>& outData, const double r ) :
                        _inData( inData ), _outData( outData ), _radius( r ),_pitch( inData.getInfo().getPitch() ) {
                        return;
                }

                void operator () ( const Point3i& p ) {
                        this->_outData.set( p, this->get_value( p ) ) ;
                        return;
                }
        private:
                char get_value( const Point3i &p ) {
                        if ( this->_inData.get( p ) == 0 ) return 0;
                        const int rx = static_cast<int>( std::ceil ( this->_radius * 1.0 / _pitch.x() ) );
                        const int ry = static_cast<int>( std::ceil ( this->_radius * 1.0 / _pitch.y() ) );
                        const int rz = static_cast<int>( std::ceil ( this->_radius * 1.0 / _pitch.z() ) );
                        for( int dz = -rz ; dz <= rz ; ++dz ) {
                                for( int dy = -ry ; dy <= ry ; ++dy ) {
                                        for( int dx = -rx ; dx <= rx ; ++dx ) {
                                                if ( _radius * _radius < dx * dx * _pitch.x() * _pitch.x() + dy * dy * _pitch.y() * _pitch.y()+ dz * dz * _pitch.z() *_pitch.z() ) continue;
                                                const Point3i np = p + Point3i( dx,dy,dz );
                                                if ( !this->_inData.getInfo().isValid( np ) ) continue;
                                                if ( this->_inData.get( np ) == 0 ) return 0;
                                        }
                                }
                        }
                        return 1;
                }
        };
}
#endif //__MI_FUNCTIONAL_ERODE_HPP__
