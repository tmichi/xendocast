/**
 * @file extract_boundary.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef __MI_FUNCTIONAL_EXTRACT_BOUNDARY_HPP__
#define __MI_FUNCTIONAL_EXTRACT_BOUNDARY_HPP__ 1
#include <functional>
#include <vector>
#include <mi/functional/is_boundary.hpp>
#include <mi/VolumeData.hpp>
#include <mi/Neighbor.hpp>
namespace mi
{
        class extract_boundary: public std::unary_function<Point3i, void>
        {
        private:
                is_boundary<char> _isBoundary;
                VolumeData<char>& _boundary;
        public:
                explicit extract_boundary( VolumeData<char>& voxel, VolumeData<char>& boundary, const int neighbor = 26 ) : _isBoundary( voxel, neighbor, 1 ), _boundary( boundary ) {
                        return;
                }
                // return true when the voxel is foreground and it contacts to any bg
                void operator () ( const Point3i& p ) const {
                        this->_boundary.at( p ) = ( this->_isBoundary( p ) ) ? 1 : 0 ;
                        return ;
                }
        };
};

#endif //__MI_FUNCTIONAL_EXTRACT_BOUNDARY_HPP__
