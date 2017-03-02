/**
 * @file  VolumeInfo.hpp
 * @author Takashi Michikawa <michiawa@acm.org>
 */
#ifndef MI_VOLUME_INFO_HPP
#define MI_VOLUME_INFO_HPP 1

#include <vector>
#include <cstdlib>
#include <cstring>
#include <iterator>

#include "math.hpp"
#include "Clamp.hpp"
namespace mi
{
        /**
         * @class VolumeInfo VolumeInfo.hpp "VolumeInfo.hpp"
         * @brief Defining size, pitch and origin point of volume data.
         */
        class VolumeInfo
        {
        private:
                void operator = ( const VolumeInfo& that );
        public:
                /**
                 * @class iterator VolumeInfo.hpp "VolumeInfo.hpp"
                 * @brief Iterator of position of volume data.
                 */
                class iterator :  public std::iterator<std::input_iterator_tag, Point3i>
                {
                public:
                        /**
                         * @brief Constructor.
                         * @param [in] info VolumeInfo.
                         * @param [in] pos Position of iterator.
                         */
                        explicit iterator( VolumeInfo* info, const bool isBegin = true ) : _info( info ) {
                                this->_pos = this->_info->getMin();
                                if( !isBegin ) this->_pos.z() = this->_info->getMax().z() + 1;
                                return;
                        }

                        /**
                         * @brief Copy constructor.
                         * @param [in] that Instance.
                         */
                        iterator( const iterator& that ) : _info( that._info ), _pos( that._pos ) {
                                return;
                        }

                        /**
                         * @brief = operator.
                         * @param [in] that Constructor.
                         * @return Instance.
                         */
                        iterator& operator = ( const iterator& that ) {
                                this->_info = that._info;
                                this->_pos  = that._pos;
                                return *this;
                        }

                        /**
                         * @brief Destructor.
                         */
                        ~iterator ( void ) {
                                return;
                        }

                        /**
                         * @brief ++ operator ( ++iter ).
                         *
                         * Proceed the position of the volume data.
                         *
                         * @return Iterator of next position.
                         */
                        iterator& operator++() {
                                const Point3i& size = this->_info->getSize();
                                if( this->_pos.z() < size.z() ) {
                                        this->_pos.x() += 1;

                                        if( this->_pos.x() < size.x() ) return *this;
                                        this->_pos.x() = 0;
                                        this->_pos.y() += 1;

                                        if( this->_pos.y() < size.y() ) return *this;
                                        this->_pos.y() = 0;
                                        this->_pos.z() += 1;
                                }
                                return *this;
                        }
                        /**
                         * @brief ++ operator ( iter++ ).
                         *
                         * Proceed the position of the volume data.
                         * @return Instance of iterator of next position.
                         */
                        iterator operator++( int ) {
                                iterator tmp ( *this );
                                ++tmp;
                                return tmp;
                        }

                        /**
                         * @brief == operator.
                         *
                         * @retval true Two iterators indicate the same position.
                         * @retval false Two iterators indicate the different  position.
                         */
                        bool operator==( const iterator& rhs ) {
                                return this->_pos == rhs._pos;
                        }

                        /**
                         * @brief != operator.
                         *
                         * @retval true Two iterators indicate the different  position.
                         * @retval false Two iterators indicate the same position.
                         */
                        bool operator!=( const iterator& rhs ) {
                                return !( this->_pos == rhs._pos );
                        }

                        /**
                         * @brief "+" operator
                         * @param [in] n The number of iterations.
                         * @return The instance.
                         */
                        iterator operator + ( const int n ) {
                                iterator tmp( *this );
                                tmp.operator+=( n );
                                return tmp;
                        }

                        /**
                         * @brief += iterator.
                         * @param [in] n The number of iteratos.
                         * @return Reference of the instance ( No instance is created.).
                         */
                        iterator& operator += ( const int n ) {
                                iterator& iter = *this;
                                for( int i = 0 ; i < n ; ++i ) ++iter;
                                return *this;
                        }

                        /**
                         * @brief *operator.
                         * @return Position where the iterator indicates.
                         */
                        Point3i& operator*( void ) {
                                return this->_pos;
                        }
                private:
                        // _info must be VolumeInfo* (not VolumeInfo&), because = operator does not work.
                        VolumeInfo* _info; ///< A pointer to the information of the volume data..
                        Point3i _pos; ///< Position where the iterator indicates.
                };//iterator
        private:
                Point3i _size;   ///< Global bounding box.
                Point3d _pitch;  ///< Voxel pitch.
                Point3d _origin; ///< Origin point. Corresponding to global (0,0,0) in voxel space.
        public:
                /**
                 * @brief Constructor.
                 * @param [in] size Size.
                 * @param [in] pitch Pitch.
                 * @param [in] origin Origin point.
                 */
                explicit VolumeInfo ( const Point3i& size = Point3i( 0,0,0 ), const Point3d& pitch = Point3d( 1,1,1 ), const Point3d& origin = Point3d( 0,0,0 ) ) : _size( size ), _pitch( pitch ), _origin( origin ) {
                        return;
                }

                explicit VolumeInfo( const VolumeInfo& that ) : _size( that._size ), _pitch( that._pitch ), _origin( that._origin ) {
                        return;
                }
                /**
                 * @brief Set size.
                 * @param [in] size Size.
                 * @return Refrence to the data.
                 */
                inline VolumeInfo& setSize( const Point3i& size ) {
                        this->_size = size;
                        return *this;
                }
                /**
                 * @brief Set pitch size.
                 * @param [in] pitch Pitch size.
                 * @return Refrence to the data.
                 */
                inline VolumeInfo& setPitch( const Point3d& pitch ) {
                        this->_pitch = pitch;
                        return *this;
                }

                /**
                 * @brief Set origin point..
                 * @param [in] origin Origin point.
                 * @return Refrence to the data.
                 */
                inline VolumeInfo& setOrigin( const Point3d& origin ) {
                        this->_origin = origin;
                        return *this;
                }
                /**
                 * @brief Get minimum posion of the bounding box.
                 * @return Minimum position (0, 0, 0).
                 */
                inline Point3i getMin ( void ) const {
                        return Point3i( 0,0,0 );
                }
                /**
                 * @brief Get maximum  posion of the bounding box.
                 * @return Maximum position (sx - 1 , sy - 1 , sz - 1).
                 */
                inline Point3i getMax ( void ) const {
                        return this->getSize() - Point3i( 1,1,1 );
                }

                /**
                 * @brief Get maximum  posion of the bounding box.
                 * @return Maximum position (sx , sy , sz).
                 */
                inline Point3i  getSize ( void ) const {
                        return this->_size;
                }

                /**
                 * @brief Get pitch size.
                 * @return Pitch size.
                 */
                inline Point3d getPitch ( void ) const {
                        return this->_pitch;
                }
                /**
                 * @brief Get origin point.
                 * @return Origin point.
                 */
                inline Point3d getOrigin ( void ) const {
                        return this->_origin;
                }

                /**
                 * @brief Get 3D position from voxel position.
                 * @param [in] p A position in voxel space.
                 * @return 3D position.
                 */
                Point3d getPointInSpace ( const Point3i& p ) const {
                        const Point3d& pitch  = this->getPitch();
                        const Point3d& origin = this->getOrigin();
                        return Point3d (
                                       origin.x() + pitch.x() * p.x(),
                                       origin.y() + pitch.y() * p.y(),
                                       origin.z() + pitch.z() * p.z()
                               );
                }

                /**
                 * @brief Get voxel position from 3D position.
                 * @param [in] p A 3D position.
                 * @return A position in voxel space.
                 */
                Point3i getPointInVoxel ( const Point3d& p ) const {
                        const Point3d& pitch  = this->getPitch();
                        const Point3d& origin = this->getOrigin();
                        return Point3i (
                                       static_cast<int>( ( p.x() - origin.x() ) / pitch.x() ),
                                       static_cast<int>( ( p.y() - origin.y() ) / pitch.y() ),
                                       static_cast<int>( ( p.z() - origin.z() ) / pitch.z() )
                               );
                }
                /**
                 * @brief Get voxel position from 3D position.
                 * @param [in] p A 3D position.
                 * @return A position in voxel space represented by Vector3d.
                 */
                Vector3d getVectorInSpace ( const Vector3s& p ) const {
                        const Point3d& pitch  = this->getPitch();
                        const Point3d& origin = this->getOrigin();
                        return Vector3d (
                                       origin.x() + pitch.x() * p.x(),
                                       origin.y() + pitch.y() * p.y(),
                                       origin.z() + pitch.z() * p.z()
                               );
                }

                /**
                 * @brief Get 1D index from a point.
                 *
                 * idx is computed by px + sx * py + sx * sy * pz.
                 * @param p Point (3D).
                 * @return Index of p (1D).
                 */
                int toIndex ( const  Point3i& p ) const {
                        if ( ! this->isValid ( p ) ) return -1;
                        const Point3i& size = this->getSize();
                        return p.x() + size.x() * ( p.y() + p.z() * size.y() ) ;
                }

                /**
                 * @brief Get 3D position from 1D index.
                 * @param idx Index.
                 * @return 3D poistion of idx.
                 */
                Point3i fromIndex ( const int idx ) const {
                        const Point3i& size = this->getSize ();
                        return Point3i (
                                       static_cast<int>(   idx %   size.x() ),
                                       static_cast<int>( ( idx % ( size.x() * size.y() ) ) / size.x() ),
                                       static_cast<int>(   idx / ( size.x() * size.y() ) )
                               );
                }

                /**
                 * @brief Check the position is inside of the volume.
                 * @param [in] p Position.
                 * @retval true p is inside.
                 * @retval false p is outside.
                 */
                bool isValid ( const Point3i & p ) const {
                        const Point3i& bmin = this->getMin ();
                        const Point3i& bmax = this->getMax ();
                        return ( bmin.x() <= p.x() && p.x() <= bmax.x() &&
                                 bmin.y() <= p.y() && p.y() <= bmax.y() &&
                                 bmin.z() <= p.z() && p.z() <= bmax.z() );
                }

                /**
                 * @brief Clamp position
                 * @param [in] p Position.
                 * @return Clamped position.
                 */
                Point3i clamp ( const Point3i& p ) const {
                        const Point3i& bmin = this->getMin ();
                        const Point3i& bmax = this->getMax ();
                        Point3i result;
                        result.x() = mi::clamp( p.x(), bmin.x(), bmax.x() );
                        result.y() = mi::clamp( p.y(), bmin.y(), bmax.y() );
                        result.z() = mi::clamp( p.z(), bmin.z(), bmax.z() );
                        return result;
                }

                /**
                 * @brief Get length of the vector.
                 * @param v Vector.
                 * @return Length of v.
                 */
                float getLength( const mi::Point3i& v ) const {
                        return static_cast<float>( std::sqrt( this->getLengthSquared( v ) ) );
                }
                /**
                 * @brief Get squared length of the vector.
                 * @param v Vector.
                 * @return Squared length of v.
                 */
                float getLengthSquared( const mi::Point3i& v ) const {
                        const mi::Point3d& p = this->getPitch();
                        return static_cast<float>( v.x()*v.x()*p.x()*p.x() + v.y()*v.y()*p.y()*p.y() + v.z()*v.z()*p.z()*p.z() );
                }

                /**
                 * @brief Get beginning iterator.
                 * @return Beginning iterator.
                 */
                iterator begin( void ) {
                        return iterator ( this, true );
                }

                /**
                 * @brief Get Ending iterator.
                 * @return Ending iterator.
                 */
                iterator end( void ) {
                        return iterator ( this, false );
                }

                /**
                 * @brief Initialize the information.
                 * @param [in] size Size of the volume data..
                 * @param [in] pitch Picth size.
                 * @param [in] origin Origin size.
                 */
                VolumeInfo& init( const Point3i& size, const Point3d& pitch,  const Point3d& origin ) {
                        return this->setSize( size ).setPitch( pitch ).setOrigin( origin );
                }


                bool isCorner ( const Point3i& p ) const {
                        if (  ! this->isValid( p ) ) return false;
                        const mi::Point3i& bmin = this->getMin();
                        const mi::Point3i& bmax = this->getMax();
                        if ( p.x() == bmin.x() ) return true;
                        if ( p.x() == bmax.x() ) return true;
                        if ( p.y() == bmin.y() ) return true;
                        if ( p.y() == bmax.y() ) return true;
                        if ( p.z() == bmin.z() ) return true;
                        if ( p.z() == bmax.z() ) return true;
                        return false;
                }

                std::string getSizeString( void ) const {
                        const mi::Point3i& size = this->getSize();
                        std::stringstream ss;
                        ss<<size.x()<<"x"<<size.y()<<"x"<<size.z();
                        return ss.str();
                }

                std::string getPitchString( void ) const {
                        const mi::Point3d& pitch = this->getPitch();
                        std::stringstream ss;
                        ss<<pitch.x()<<"x"<<pitch.y()<<"x"<<pitch.z();
                        return ss.str();
                }
        };
};

#endif//
