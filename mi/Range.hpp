#ifndef MI_RANGE_HPP
#define MI_RANGE_HPP 1
#include "math.hpp"
#include <iterator>
namespace mi
{
        class Range
        {
        public:
                class iterator :  public std::iterator<std::input_iterator_tag, Point3i>
                {
                public:
                        /**
                         * @brief Constructor.
                         * @param [in] range Range.
                         * @param [in] isBeginning
                         */
                        explicit iterator( Range* range, const bool isBeginning = true ) : _range( range ) {
                                this->_pos = range->getMin();
                                if ( !isBeginning ) this->_pos.z() = range->getMax().z() + 1;
                                return;
                        }

                        /**
                         * @brief Copy constructor.
                         * @param [in] that Instance.
                         */
                        iterator( const iterator& that ) : _range( that._range ), _pos( that._pos ) {
                                return;
                        }

                        iterator&  operator = ( const iterator& that ) {
                                this->_range = that._range;
                                this->_pos = that._pos;
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
                         * @return Iterator of next position.
                         */
                        iterator& operator++() {
                                const Point3i& bmin = this->_range->getMin();
                                const Point3i& bmax = this->_range->getMax();
                                Point3i& pos = this->_pos;
                                if( pos.z() <= bmax.z() ) {
                                        pos.x() += 1;
                                        if( bmax.x() < pos.x() ) {
                                                pos.x() = bmin.x();
                                                pos.y() += 1;
                                                if( bmax.y() < pos.y() ) {
                                                        pos.y() = bmin.y();
                                                        pos.z() += 1;
                                                }
                                        }
                                }

                                return *this;
                        }
                        /**
                         * @brief ++ operator ( iter++ ).
                         * @return Instance of iterator of next position.
                         */
                        iterator operator++( int ) {
                                iterator tmp ( *this );
                                ++tmp;
                                return tmp;
                        }

                        /**
                         * @brief == operator.
                         * @retval true Two iterators indicate the same position.
                         * @retval false Two iterators indicate the different  position.
                         */
                        bool operator==( const iterator& rhs ) {
                                return this->_pos == rhs._pos;
                        }

                        /**
                         * @brief != operator.
                         * @retval true Two iterators indicate the different  position.
                         * @retval false Two iterators indicate the same position.
                         */
                        bool operator != ( const iterator& rhs ) {
                                return this->_pos != rhs._pos;
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
                        Range*  _range; // Pointer to the range.
                        Point3i _pos;   // Current position.
                };//iterator
        public:
                /**
                 * @brief Constructor.
                 * @param [in] bmin Bounding box (min).
                 * @param [in] bmax Bounding box (max).
                 */
                explicit Range( const Point3i& bmin, const Point3i& bmax ) {
                        this->_bmin = bmin;
                        this->_bmax = bmax;
                        if( bmin.x() > bmax.x() ) {
                                this->_bmin.x() = bmax.x();
                                this->_bmax.x() = bmin.x();
                        }
                        if( bmin.y() > bmax.y() ) {
                                this->_bmin.y() = bmax.y();
                                this->_bmax.y() = bmin.y();
                        }
                        if( bmin.z() > bmax.z() ) {
                                this->_bmin.z() = bmax.z();
                                this->_bmax.z() = bmin.z();
                        }
                        return;
                }

                Range( const Range& that ) : _bmin( that._bmin ), _bmax( that._bmax ) {
                        return;
                }

                ~Range ( void ) {
                        return;
                }

                Range& operator = ( const Range& that ) {
                        this->_bmin = that._bmin;
                        this->_bmax = that._bmax;
                        return *this;
                }

                bool isValid ( const Point3i& p ) const {
                        return ( this->_bmin.x() <= p.x() && p.x() <= this->_bmax.x() &&
                                 this->_bmin.y() <= p.y() && p.y() <= this->_bmax.y() &&
                                 this->_bmin.z() <= p.z() && p.z() <= this->_bmax.z() );
                }

                inline Point3i getMin( void ) const {
                        return this->_bmin;
                }

                inline Point3i getMax( void ) const {
                        return this->_bmax;
                }

                iterator begin( void ) {
                        return iterator( this, true );
                }

                iterator end( void ) {
                        return iterator( this, false );
                }
        private:
                Point3i _bmin;
                Point3i _bmax;
        };
}
#endif //MI_RANGE_HPP
