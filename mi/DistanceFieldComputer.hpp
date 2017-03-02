/**
 * @file DistanceFieldComputer.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_DISTANCE_FIELD_COMPUTER_HPP
#define MI_DISTANCE_FIELD_COMPUTER_HPP 1

#include <functional>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>

#include "math.hpp"
#include "ParallelFor.hpp"
#include "VolumeData.hpp"
#include "Limits.hpp"

namespace mi
{
        class vec2dist: public std::unary_function<Point3i, void>
        {
        private:
                const VolumeData<Vector3s>& _dfData;
                VolumeData<float>& _df;
                const Point3d _pitch;
        public:
                vec2dist ( const VolumeData<Vector3s>& dfData, VolumeData<float>& df ) : _dfData( dfData ), _df( df ), _pitch( df.getInfo().getPitch() ) {
                        return;
                }

                void operator () ( const  Point3i& p ) {
                        this->_df.set( p, this->get_dist( this->_dfData.get( p ), this->_pitch ) );
                        return;
                }
        private:
                inline float get_dist( const Vector3s& v, const Point3d& p ) const {
                        return std::sqrt( static_cast<float>( v.x() * v.x() * p.x() * p.x() +
                                                              v.y() * v.y() * p.y() * p.y() +
                                                              v.z() * v.z() * p.z() * p.z() ) );
                }
        };

        class DistanceFieldComputer
        {
        private:
                DistanceFieldComputer ( const DistanceFieldComputer& that );
                void operator =  ( const DistanceFieldComputer& that );

                class compute_distance_field : public std::unary_function<int, void>
                {
                private:
                        const VolumeData<char>& _binary;
                        VolumeData<Vector3s>& _data;
                public:
                        compute_distance_field ( const VolumeData<char>& binary,  VolumeData<Vector3s>& data ) : _binary ( binary ), _data ( data ) {
                                return;
                        }

                        void operator () ( const int z ) {
                                const int MAX_VALUE = std::numeric_limits<short>::max();
                                const VolumeInfo& info = this->_data.getInfo();

                                const Point3d& pitch = info.getPitch();
                                const Point3i& size  = info.getSize();

                                std::vector< std::vector<short> > cloz ( size.y() , std::vector<short>( size.x(), -MAX_VALUE ) ); //closest points on z direction
                                for( int y = 0 ; y < size.y() ; ++y ) {
                                        for( int x = 0 ; x < size.x() ; ++x ) {
                                                if( this->_binary.get( x,y,z ) == 0 ) cloz[y][x] = z;
                                                else {
                                                        float mind = std::numeric_limits<float>::max();
                                                        for( int dz = 0 ; dz < size.z() ; ++dz ) {
                                                                if ( this->_binary.get( x,y,dz ) == 0 ) {
                                                                        Vector3s v ( 0, 0, dz - z );
                                                                        float d = get_dist2( v, pitch );
                                                                        if( d  <  mind ) {
                                                                                mind = d;
                                                                                cloz[y][x] = static_cast<short>( dz );
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                }
                                for( int y = 0 ; y <  size.y() ; ++y ) {
                                        std::vector<short> cloy ( size.x(), -MAX_VALUE ) ;
                                        for( int x = 0 ; x < size.x() ; ++x ) {
                                                if( this->_binary.get( x,y,z ) == 0 ) cloy[x] = y;
                                                else {
                                                        float mind = std::numeric_limits<float>::max();
                                                        for( int dy = 0 ; dy < size.y() ; ++dy ) {
                                                                if ( cloz[dy][x] < 0 ) continue; // when closest site does not exist.
                                                                Vector3s v ( 0, dy - y, cloz[dy][x] - z ); // (0, dy, dz)
                                                                float d = get_dist2( v, pitch );	 //dy * dy + dz * dz
                                                                if( d < mind ) {
                                                                        mind = d;
                                                                        cloy[x] = dy;
                                                                }
                                                        }
                                                }
                                        }
                                        for( int x = 0 ; x < size.x() ; ++x ) {
                                                short clox = -MAX_VALUE;
                                                if( this->_binary.get( x,y,z ) == 0 ) {
                                                        clox = x;
                                                } else {
                                                        float mind = std::numeric_limits<float>::max();
                                                        for( int dx = 0 ; dx < size.x() ; ++dx ) {
                                                                if ( cloy[dx] < 0 ) continue;
                                                                Vector3s v ( dx - x , cloy[dx] - y, cloz[cloy[dx]][dx] - z );
                                                                float d = get_dist2( v,  pitch );
                                                                if( d < mind ) {
                                                                        mind = d;
                                                                        clox = dx;
                                                                }
                                                        }
                                                }


                                                const short cx = clox;
                                                const short cy = cloy[cx];
                                                const short cz = cloz[cy][cx];

                                                Vector3s result( cx - x, cy - y, cz - z );


                                                this->_data.set( x, y, z, result );
                                        }
                                }
                                return;
                        }
                private:
                        inline float get_dist2( const Vector3s& v, const Point3d& p ) const {
                                return static_cast<float>( v.x() * v.x() * p.x() * p.x() +
                                                           v.y() * v.y() * p.y() * p.y() +
                                                           v.z() * v.z() * p.z() * p.z() );
                        }

                        inline float get_dist( const Vector3s& v, const Point3d& p ) const {
                                return std::sqrt( this->get_dist2( v,p ) );
                        }
                };
        private:
                const VolumeData<char>& _binary;
                VolumeData<Vector3s>& _data;
        public:
                /**
                 * @param [in] binary Binary image.
                 * @param [in] data Reesult data ( represented by vector.)
                 * @todo Remove third parameter from the arguments.
                 */
                explicit DistanceFieldComputer ( const VolumeData<char>& binary, VolumeData<Vector3s>& data ) : _binary( binary ), _data( data ) {
                        return;
                }

                ~DistanceFieldComputer ( void ) {
                        return;
                }

                /**
                 * @param [in] num_thread The number of threads.
                 */
                bool compute ( const int num_thread = 1 ) {
                        const Point3i& size = this->_data.getInfo().getSize();
                        std::vector<int> zarray;
                        for( int i = 0 ; i < size.z() ; ++i ) zarray.push_back( i );
                        int grainSize = static_cast<int>( size.z() * 1.0 / num_thread );
                        if ( grainSize == 0 ) grainSize = 1;
                        parallel_for_each ( zarray.begin(), zarray.end(), compute_distance_field( this->_binary, this->_data ), grainSize );
                        return true;
                }

                bool compute2d( void ) {
                        compute_distance_field cdf( this->_binary, this->_data );
                        cdf( 0 );
                        return true;

                }
        };
};
#endif
