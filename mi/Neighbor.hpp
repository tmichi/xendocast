#ifndef MI_NEIGHBOR_HPP
#define MI_NEIGHBOR_HPP 1
#include <vector>
#include <algorithm>
#include <functional>
#include "math.hpp"

namespace mi
{
        /**
         * @class Neighbor Neighbor.hpp <mi/Neighbor.hpp>
         * @brief Vector to neighboring voxels.
         */
        class Neighbor
        {
        private:
                Neighbor( const Neighbor& that );
                void operator = ( const Neighbor& that );
        public:
                typedef std::vector<Point3i>::iterator iterator;
        private:
                class l1dist : std::unary_function<Point3i, bool>
                {
                public:
                        result_type  operator() ( const Point3i& lhs, const Point3i& rhs ) const {
                                return  lhs.x() * lhs.x() + lhs.y() * lhs.y() + lhs.z() * lhs.z() <
                                        rhs.x() * rhs.x() + rhs.y() * rhs.y() + rhs.z() * rhs.z();
                        }
                };
        public:
                Neighbor( void ) {
                        return;
                }
                static std::vector< Point3i >& getNeighbor( void ) {
                        static std::vector< Point3i > dp;
                        if( dp.size() == 0 ) {
                                dp.reserve( 26 );
                                for( int dz = -1 ; dz <= 1 ; ++dz ) {
                                        for( int dy = -1 ; dy <= 1 ; ++dy ) {
                                                for( int dx = -1 ; dx <= 1 ; ++dx ) {
                                                        if( dx == 0 && dy == 0 && dz == 0 ) continue;
                                                        dp.push_back( Point3i( dx, dy, dz ) );
                                                }
                                        }
                                }
                                std::sort( dp.begin(), dp.end(), l1dist() );
                        }
                        return dp;
                }

                static iterator begin( void ) {
                        return Neighbor::getNeighbor().begin();
                }

                static iterator end ( const int n = 26 ) {
                        const int nbr = ( n == 6 || n == 18 || n == 26 ) ? n : 26;
                        return Neighbor::getNeighbor().begin() + nbr;
                }

                static iterator end6 ( void ) {
                        return Neighbor::end( 6 );
                }

                static iterator end18 ( void ) {
                        return Neighbor::end( 18 );
                }

                static iterator end26 ( void ) {
                        return Neighbor::end( 26 );
                }
        };
}
#endif //MI_NEIGHBOR_HPP
