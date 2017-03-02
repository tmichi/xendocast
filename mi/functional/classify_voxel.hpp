/**
 * @file VoxelTypeClassifier.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_CLASSIFY_VOXEL_HPP__
#define __MI_FUNCTIONAL_CLASSIFY_VOXEL_HPP__ 1

#include <iostream>
#include <vector>

#include <string>
#include <cstdlib>
#include <vector>
#include <queue>
#include <cassert>
#include <iostream>
#include <string>
#include <cstdlib>

#include <mi/VoxelType.hpp>
#include <mi/ComponentCounter.hpp>
#include <mi/Clamp.hpp>

namespace mi
{
        /**
         * @class VoxelTypeClassifier VoxelTypeClassifier.hpp "mi/VoxelTypeClassifier.hpp"
         */
        class VoxelTypeClassifier
        {
        private:
                enum NeighborType {
                        NEIGHBOR_NONE, // none
                        NEIGHBOR_26,
                        NEIGHBOR_18,
                        NEIGHBOR_6
                };
                /**
                 * @class ComponentCounter ComponentCounter.hpp "mi/ComponentCounter.hpp"
                 * @todo This implementation supposes 3x3x3 voxels. This should be improved to general cases.
                 */
                class ComponentCounter
                {
                private:
                        int _center;
                        std::vector<bool> _voxel;
                public:
                        ComponentCounter ( const std::vector<bool>& voxel ) : _center ( voxel.size() -1 ), _voxel ( voxel ) {
                                if ( voxel.size() != 27 ) {
                                        std::cerr<<"size error of ComponentCounter "<<std::endl;
                                }
                                return;
                        }

                        virtual ~ComponentCounter ( void ) {
                                return;
                        }

                        char countBackgroundComponents ( NeighborType type = NEIGHBOR_26, bool ignoreCenter = false ) {
                                std::vector<bool> voxel ( 27,false );
                                for ( int i = 0 ; i < 27 ; i+= 1 ) voxel[i] = !_voxel[i] ;
                                if ( ignoreCenter ) voxel[13] = false;
                                return this->count ( voxel, type );
                        }

                        char countForegroundComponents ( NeighborType type = NEIGHBOR_26, bool ignoreCenter = false ) {
                                std::vector<bool> voxel ( 27,false );
                                for ( int i = 0 ; i < 27 ; i+= 1 ) voxel[i] = _voxel[i];
                                if ( ignoreCenter ) voxel[13] = false;
                                return this->count ( voxel, type );
                        }
                private:
                        char count ( const std::vector<bool>& bit, const NeighborType type ) const {
                                std::vector<char> label ( 27, -1 );
                                const char DEFAULT_VALUE = 28;

                                std::queue<char> snbr;
                                for ( int i = 0 ; i < 27 ; i+=1 ) {
                                        if ( bit[i] ) {
                                                label[i] = DEFAULT_VALUE;
                                                snbr.push ( i );
                                        }
                                }
                                char label_count = 0;
                                while ( !snbr.empty() ) {
                                        const int target = snbr.front();
                                        snbr.pop();
                                        std::queue<int> q;
                                        q.push ( target );
                                        while ( !q.empty() ) {
                                                const int top = q.front();
                                                q.pop();
                                                if ( label[top] != DEFAULT_VALUE ) continue;
                                                label[top] = label_count;

                                                const int zz = static_cast<int> ( top / 9 ) ;
                                                const int yy = static_cast<int> ( ( top % 9 ) / 3 );
                                                const int xx = top % 3;

                                                for ( int z = -1 ; z <= 1 ; z+=1 ) {
                                                        if ( zz + z < 0 || 2 < zz + z ) continue;
                                                        for ( int y = -1 ; y <= 1 ; y += 1 ) {
                                                                if ( yy + y  < 0 || 2 < yy + y ) continue;
                                                                for ( int x = -1 ; x <= 1 ; x += 1 ) {
                                                                        if ( xx + x < 0 || 2 < xx + x ) continue;

                                                                        int dist = std::abs ( x ) + std::abs ( y ) + std::abs ( z );
                                                                        if ( dist == 0 ) continue;
                                                                        if ( type == NEIGHBOR_6  && dist > 1 ) continue; //6 neigbhor
                                                                        if ( type == NEIGHBOR_18 && dist > 2 ) continue; //18 neigbhor

                                                                        const int id = ( zz + z ) * 9 + ( yy + y ) * 3 + ( xx + x );
                                                                        if ( label[id] == DEFAULT_VALUE ) { // if the voxel is no labelled.
                                                                                q.push ( id );
                                                                        }
                                                                }
                                                        }
                                                }
                                        }
                                        label_count += 1;

                                        while ( !snbr.empty() ) {
                                                if ( label[snbr.front() ] == DEFAULT_VALUE ) break;
                                                snbr.pop();
                                        }
                                }
                                return label_count;
                        }
                }; // class ComonentCounter

        private:
                VoidTypeClassifier ( VoxelTypeClassifier &that );
                void operator =    ( VoxelTypeClassifier &that );
        private:
                VolumeData<char>& _voxel;
        public:
                VoidTypeClassifier ( VolumeData<char>& voxel ) : _voxel( voxel ) {
                        return;
                }

                virtual ~VoxelTypeClassifier ( void ) {
                        return ;
                }

                VoxelType get ( const Point3i& p ) {
                        const char bgValue = 0;
                        const VolumeInfo &info =  this._voxel.getInfo();
                        if( !info.isValid( p ) ) return Invalid; // return Invalid if position is invalid
                        if( this._voxel.get( p ) == bgValue ) return Background;
                        std::vector<bool> bit;
                        bit.reserve ( 27 );
                        for( int dz = -1 ; dz <= 1 ; ++dz ) {
                                for( int dy = -1 ; dy <= 1 ; ++dy ) {
                                        for( int dx = -1 ; dx <= 1 ; ++dx ) {
                                                Point3i np = p + Point3i( dx, dy, dz );
                                                Point3i cp = info.clamp( np );
                                                bit.push_back( voxel.get( cp ) != bgValue );
                                        }
                                }
                        }

                        mi::ComponentCounter counter ( bit );
                        const char bgnum = counter.countBackgroundComponents ( NEIGHBOR_18, false );
                        const char fgnum = counter.countForegroundComponents ( NEIGHBOR_26, true );

                        if      ( bgnum == 0 )               return InteriorPoint;
                        else if ( fgnum == 0 )               return IsolatePoint;
                        else if ( bgnum == 1 && fgnum == 1 ) return BorderPoint;
                        else if ( bgnum == 1 && fgnum == 2 ) return CurvePoint;
                        else if ( bgnum == 1 && fgnum >  2 ) return CurvesJunction;
                        else if ( bgnum == 2 && fgnum == 1 ) return SurfacePoint;
                        else if ( bgnum == 2 && fgnum >= 2 ) return SurfaceCurveJunction;
                        else if ( bgnum >  2 && fgnum == 1 ) return SurfacesJunction;
                        else if ( bgnum >  2 && fgnum >= 2 ) return SurfacesCurvesJunction;
                        else	                             return Invalid;
                }

        };
}
#endif //__MI_FUNCTIONAL_CLASSIFY_VOXEL_HPP__

