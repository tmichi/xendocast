/**
 * @file dilate.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_DILATE_HPP__
#define __MI_FUNCTIONAL_DILATE_HPP__ 1
#include <functional>
#include <mi/VolumeData.hpp>
#include <mi/math.hpp>
namespace mi
{
        class dilate
        {
        private:
                VolumeData<char>& _inData;
                VolumeData<char>& _outData;
                const double _radius;
                Point3d _pitch; // (x*x, y*y, z*z)
        public:
                dilate ( VolumeData<char>& inData, VolumeData<char>& outData, const double r ) :
                        _inData( inData ), _outData( outData ), _radius( r ), _pitch( inData.getInfo().getPitch() ) {

                        const int rx = static_cast<int>( std::ceil ( this->_radius * 1.0 / _pitch.x() ) );
                        const int ry = static_cast<int>( std::ceil ( this->_radius * 1.0 / _pitch.y() ) );
                        const int rz = static_cast<int>( std::ceil ( this->_radius * 1.0 / _pitch.z() ) );
                        std::cerr<<"size"<<rx<<" "<<ry<<" "<<rz<<std::endl;
                        return;
                }

                void operator () ( const Point3i& p ) {
                        this->_outData.set( p, this->get_value( p ) ) ;
                        return;
                }
        private:
                char get_value( const Point3i &p ) {
                        if ( this->_inData.get( p ) == 1 ) return 1;
                        const int rx = static_cast<int>( std::ceil ( this->_radius * 1.0 / _pitch.x() ) );
                        const int ry = static_cast<int>( std::ceil ( this->_radius * 1.0 / _pitch.y() ) );
                        const int rz = static_cast<int>( std::ceil ( this->_radius * 1.0 / _pitch.z() ) );
                        for( int dz = -rz ; dz <= rz ; ++dz ) {
                                double vz = dz * _pitch.z();
                                for( int dy = -ry ; dy <= ry ; ++dy ) {
                                        double vy = dy * _pitch.y();
                                        for( int dx = -rx ; dx <= rx ; ++dx ) {
                                                double vx = dx * _pitch.x();
                                                const Point3i np = p + Point3i( dx,dy,dz );
                                                if ( !this->_inData.getInfo().isValid( np ) ) continue;
                                                if ( _radius * _radius < vx * vx + vy * vy + vz * vz ) continue;
                                                if ( this->_inData.get( np ) == 1 ) return 1;
                                        }
                                }
                        }
                        return 0;
                }
        };
};
#endif //__MI_FUNCTIONAL_DILATE_HPP__
