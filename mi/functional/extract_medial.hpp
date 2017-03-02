/**
 * @file extract_medial.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_EXTRACT_MEDIAL_HPP__
#define __MI_FUNCTIONAL_EXTRACT_MEDIAL_HPP__ 1
#include <cmath>
#include <functional>
#include <mi/math.hpp>
#include <mi/VolumeData.hpp>
#include <mi/Neighbor.hpp>
#include <mi/PriorityQueue.hpp>

namespace mi
{
        /*
          if ( dist < maxGeodesicDistance ) {
          const Point3i nbp0 = p0 + Point3i( v0.x(), v0.y(), v0.z() );
          const Point3i nbp1 = p1 + this->toPoint3i( v1 );
          dist = gdc.getDistance(nbp0, nbp1);
          if( maxDist < dist ) maxDist = dist ;
          }
        */

        class GeodesicDistanceComputer
        {
        private:
                VolumeData<char>& _data;
                float _threshold;

                GeodesicDistanceComputer( const GeodesicDistanceComputer& that );
                void operator = ( const GeodesicDistanceComputer& that );
        public:
                GeodesicDistanceComputer( VolumeData<char>& data, const float threshold ) : _data( data ), _threshold( threshold ) {
                        return ;
                }


                ~GeodesicDistanceComputer( void ) {
                        return ;
                }

                float getDistance( const Point3i& pS, const Point3i& pT ) {
                        VolumeInfo& info = this->_data.getInfo();
                        ///const Point3d& pitch = info.getPitch();

                        VolumeData<char> freeze( info );
                        VolumeData<float> dist( info );
                        PriorityQueue<Point3i> pq;

                        const float MAX_DIST = 10000000;
                        for( VolumeInfo::iterator iter = info.begin() ; iter != info.end() ; ++iter ) {
                                dist.at( *iter ) = ( this->_data.get( *iter ) == 1 ) ? MAX_DIST : 0;
                        }
                        // set 0 to init
                        pq.push( pS, 0 );
                        dist.set( pS, 0 );

                        float topDist = 0;
                        while ( !pq.empty() ) {
                                const Point3i p = pq.getTopIndex();
                                topDist = pq.getTopCost();
                                if ( this->_threshold <= topDist ) return this->_threshold;
                                if ( p == pT ) return topDist;
                                pq.pop();
                                if( freeze.get( p ) == 1 ) continue;
                                freeze.set( p, 1 );

                                static Neighbor nbr;
                                for( Neighbor::iterator iter = nbr.begin() ; iter != nbr.end( 26 ); ++iter ) {
                                        const Point3i dp = *iter;
                                        const Point3i np = p + dp;
                                        if ( !info.isValid ( np ) ) continue; //
                                        if ( this->_data.get( np ) == 0 ) continue; // the voxel is background
                                        if ( freeze.get( np ) == 1 ) continue;

                                        const float ldist = info.getLength( dp );
                                        const float ndist = topDist + ldist;
                                        const float odist = dist.get( np );
                                        if( ndist < odist ) {
                                                dist.set( np, ndist );
                                                pq.push( np, ndist );
                                        }
                                }
                        }
                        return MAX_DIST;
                }
        };
        class extract_medial : public std::unary_function<Point3i, bool>
        {
        public:
                extract_medial( VolumeData<Vector3s>& df, const float threshold, VolumeData<char>& gdf ) : _df( df ), _threshold( threshold ), _gdf( gdf ) {
                        return;
                }

                bool operator () ( const Point3i& p0 ) {
                        this->_gdf.set( p0, 0 );
                        VolumeInfo& info = this->_gdf.getInfo();
                        if( this->is_bg( p0 ) ) return false;

                        float maxDist = 0;

                        Vector3s v0 = this->_df.get( p0 );
                        Point3i nbp0 = Point3i ( v0.x(), v0.y(), v0.z() ) + p0;
                        Vector3d nbv0 = info.getVectorInSpace( v0  );
                        const float length = static_cast<float>( nbv0.norm() );
                        nbv0.normalize();

                        Neighbor::iterator begin = Neighbor::begin();
                        Neighbor::iterator end = Neighbor::end( 26 );
                        for( Neighbor::iterator niter = begin ; niter != end ; ++niter ) {
                                const Point3i p1 = p0 + *niter;
                                if ( this->is_bg( p1 ) ) continue;

                                Vector3s v1 = this->_df.get( p1 );
                                Point3i nbp1 = Point3i ( v1.x(), v1.y(), v1.z() ) + p1;
                                if( nbp0 == nbp1 ) continue;
                                Vector3d nbv1 = info.getVectorInSpace( this->_df.get( p1 ) ) ;
                                nbv1.normalize();

                                const float angle = static_cast<float>( std::acos( nbv0.dot( nbv1 ) ) ) ;
                                const float dist =  length * angle ;
                                if( dist > maxDist ) maxDist = dist ;
                                if( this->_threshold <= maxDist ) {
                                        this->_gdf.set( p0, 1 );
                                        return true;
                                }
                        }
                        // step 2 dijkstra.


                        return false;
                }
        private:
                inline bool is_bg( const Point3i& p ) {
                        if ( !this->_df.getInfo().isValid( p ) ) return true;
                        return ( this->_df.get( p ) == Vector3s() );
                }
        private:
                VolumeData<Vector3s>& _df;
                const float _threshold;
                VolumeData<char>& _gdf;
        };

        class extract_medial_scalar : public std::unary_function<Point3i, bool>
        {
        public:
                extract_medial_scalar( VolumeData<Vector3s>& df, const double threshold, VolumeData<float>& gdf ) : _df( df ), _threshold( threshold ), _gdf( gdf ) {
                        return;
                }

                bool operator () ( const Point3i& p0 ) {
                        VolumeInfo& info = this->_gdf.getInfo();
                        if( this->is_bg( p0 ) ) return false;
                        this->_gdf.set( p0, 0 );
                        float maxDist = 0;
                        Vector3d nbv0 = info.getVectorInSpace( this->_df.get( p0 ) );
                        const float length = static_cast<float>( nbv0.norm() );
                        nbv0.normalize();
                        Neighbor::iterator begin = Neighbor::begin();
                        Neighbor::iterator end = Neighbor::end( 26 );
                        for( Neighbor::iterator niter = begin ; niter != end ; ++niter ) {
                                const Point3i p1 = p0 + *niter;
                                if ( this->is_bg( p1 ) ) continue;
                                Vector3d nbv1 = info.getVectorInSpace( this->_df.get( p1 ) ) ;
                                nbv1.normalize();

                                const float angle = static_cast<float>( std::acos( nbv0.dot( nbv1 ) ) ) ;
                                const float dist =  static_cast<float>( length * angle ) ;
                                if( dist > maxDist ) maxDist = dist ;

                                if( this->_threshold <= maxDist ) {
                                        Vector3s v = this->_df.get( p0 );
                                        float dist = info.getLength ( Point3i( v.x(), v.y(), v.z() ) );
                                        this->_gdf.set( p0,  dist );
                                        return true;
                                }
                        }
                        return false;
                }
        private:
                inline bool is_bg( const Point3i& p ) {
                        if ( !this->_df.getInfo().isValid( p ) ) return true;
                        return ( this->_df.get( p ) == Vector3s() );
                }

        private:
                VolumeData<Vector3s>& _df;
                const double _threshold;
                VolumeData<float>& _gdf;
        };

};
#endif //__MI_FUNCTIONAL_EXTRACT_MEDIAL_HPP__
