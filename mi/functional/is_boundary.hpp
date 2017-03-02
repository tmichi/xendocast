/**
 * @file is_boundary.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_IS_BOUNDARY_HPP__
#define __MI_FUNCTIONAL_IS_BOUNDARY_HPP__ 1
#include <functional>
#include <vector>
#include <mi/VolumeData.hpp>
#include <mi/Neighbor.hpp>
namespace mi
{

        template<typename T>
        class is_boundary : public std::unary_function<Point3i, bool>
        {
        private:
                VolumeData<T>& _voxel;
                const int _neighbor;
                const T _bgValue;
        public:
                explicit is_boundary( VolumeData<T>& voxel, const int neighbor = 26, const T bgValue = T() ) :  _voxel( voxel ), _neighbor( neighbor ), _bgValue( bgValue ) {
                        return;
                }

                // return true when the voxel is foreground and it contacts to any bg
                bool  operator () ( const Point3i& p ) const {
                        if ( ! this->_voxel.getInfo().isValid( p ) ) return false; // Invalid voxel is not boundary voxel.
                        if ( ! this->is_bg( p ) ) return false; // foreground voxel is not also boundary voxel.

                        Neighbor::iterator begin = Neighbor::begin();
                        Neighbor::iterator end   = Neighbor::end( this->_neighbor );
                        for( Neighbor::iterator iter = begin ; iter != end ; ++iter ) {
                                const Point3i np = p + ( *iter );
                                if ( ! this->_voxel.getInfo().isValid( np ) ) continue;
                                if ( ! this->is_bg( np ) ) {
                                        return true;
                                }
                        }
                        return false;
                }
        private:
                inline bool is_bg( const Point3i& p ) const {
                        return ( this->_voxel.get( p ) == this->_bgValue );
                }
        };
};
#endif // __MI_FUNCTIONAL_IS_BOUNDARY_HPP__
