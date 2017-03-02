/**
 * @file VolumeDataCreator.hpp
 * @author Takashi Michikawa
 */
#ifndef MI_VOLUME_DATA_CREATOR_HPP
#define MI_VOLUME_DATA_CREATOR_HPP 1
#include "VolumeData.hpp"
namespace mi
{
        /**
         * @class VolumeDataCreator VolumeDataCreator.hpp "VolumeDataCreator.hpp"
         * @brief Utility for creating volume data.
         */
        template <typename T>
        class VolumeDataCreator
        {
        private:
                VolumeDataCreator ( const VolumeDataCreator& that );
                void operator = ( const VolumeDataCreator& that );
        private:
                VolumeData<T>& _data; ///< Volume data.
                T _currentValue;      ///< Current value.
        public:
                /**
                 * @brief Constructor.
                 * @param [in] data Volume data.
                 * @param [in] value Initial value.
                 */
                explicit VolumeDataCreator( VolumeData<T>& data, const T value = T() ) : _data( data ) {
                        this->setValue( value );
                        this->fill();
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                ~VolumeDataCreator( void ) {
                        return;
                }

                /**
                 * @brief Set value.
                 * @param [in] value Value.
                 * @return A reference to the object.
                 */
                inline VolumeDataCreator<T>& setValue( const T value ) {
                        this->_currentValue = value;
                        return *this;
                }

                /**
                 * @brief Get value.
                 * @return Current value.
                 */
                T getValue ( void ) const {
                        return this->_currentValue;
                }

                /**
                 * @brief Fill by value.
                 * @return A reference to the object.
                 */
                VolumeDataCreator<T>& fill ( void ) {
                        const Point3i& bmin = this->_data.getInfo().getMin();
                        const Point3i& bmax = this->_data.getInfo().getMax();
                        this->fillBlock( bmin, bmax );
                        return *this;
                }
                /**
                 * @brief Fill by sphere.
                 * @param [in] p Center point.
                 * @param [out] rad Radius.
                 * @return A reference to the object.
                 */
                VolumeDataCreator<T>& fillSphere ( const Point3i& p, const double rad ) {
                        const mi::Vector3d& pitch = this->_data.getInfo().getPitch();
                        const int rx = static_cast<int>( std::ceil( rad / pitch.x() ) );
                        const int ry = static_cast<int>( std::ceil( rad / pitch.y() ) );
                        const int rz = static_cast<int>( std::ceil( rad / pitch.z() ) );
                        const double radSqr = rad * rad;

                        for( int z = -rz ; z <= rz ; ++z ) {
                                for( int y = -ry ; y <= ry ; ++y ) {
                                        for( int x = -rx ; x <= rx; ++x ) {
                                                if( this->_data.getInfo().getLengthSquared( mi::Point3i( x,y,z ) )  <= radSqr ) {
                                                        this->fillPoint(  p + Point3i( x,y,z ) );
                                                }
                                        }
                                }
                        }
                        return *this;
                }
                /**
                 * @brief Draw point
                 * @param [in] p Center point.
                 * @return A reference to the object.
                 */
                inline VolumeDataCreator<T>& fillPoint( const Point3i& p ) {
                        if( this->_data.getInfo().isValid( p ) ) {
                                this->_data.set( p, this->getValue() );
                        }
                        return *this;
                }
                /**
                 * @brief Draw point
                 * @param [in] bmin Bounding box (min).
                 * @param [in] bmax Bounding box (max).
                 * @return A reference to the object.
                 */
                inline VolumeDataCreator<T>& fillBlock ( const Point3i& bmin,  const Point3i& bmax ) {
                        for( int z = bmin.z() ; z <= bmax.z() ; ++z ) {
                                for( int y = bmin.y() ; y <= bmax.y() ; ++y ) {
                                        for( int x = bmin.x() ; x <= bmax.x() ; ++x ) {
                                                const Point3i p( x,y,z );
                                                this->fillPoint( p );
                                        }
                                }
                        }
                        return *this;
                }
        };
}
#endif// MI_VOLUME_DATA_CREATOR_HPP
