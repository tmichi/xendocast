/**
 * @file VolumeData.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_VOLUME_DATA_HPP
#define MI_VOLUME_DATA_HPP 1
#include <iterator>
#include <iostream>
#include "VolumeInfo.hpp"

namespace mi
{
        /**
         * @class VolumeData VolumeData.hpp <mi/VolumeData.hpp>
         */
        template <typename T>
        class VolumeData
        {
        public :
                class iterator :  public std::iterator<std::input_iterator_tag, T>
                {
                private:
                        std::vector<std::vector<std::vector<T> > >* _data;
                        typename std::vector<T>::iterator _iterX;
                        typename std::vector<std::vector<T> >::iterator _iterY;
                        typename std::vector<std::vector<std::vector<T> > >::iterator _iterZ;
                public:
                        explicit iterator( std::vector<std::vector<std::vector<T> > >* data, bool isBegin = true ) : _data( data ) {
                                if( isBegin ) {
                                        this->_iterZ = this->_data->begin();
                                        if( this->_iterZ != this->_data->end() ) {
                                                this->_iterY = this->_iterZ->begin();
                                                if( this->_iterY != this->_iterZ->end() ) {
                                                        this->_iterX = this->_iterY->begin();
                                                }
                                        }
                                } else {
                                        this->_iterZ = this->_data->end();
                                }
                                return;
                        }

                        iterator( const iterator& that ) : _data( that._data ), _iterX( that._iterX ), _iterY( that._iterY ), _iterZ( that._iterZ ) {
                                return;
                        }

                        iterator& operator = ( const iterator& that ) {
                                this->_data = that._data;
                                this->_iterX = that._iterX;
                                this->_iterY = that._iterY;
                                this->_iterZ = that._iterZ;
                                return *this;
                        }

                        ~iterator ( void ) {
                                return;
                        }

                        iterator& operator++() {
                                if( this->_iterZ == this->_data->end() ) return *this;
                                //if( this->_iterY == this->_iterZ->end() ) return *this;
                                //if( this->_iterX == this->_iterY->end() ) return *this;

                                ++ ( this->_iterX );
                                if( this->_iterX == this->_iterY->end() ) {
                                        ++ ( this->_iterY );
                                        if( this->_iterY == this->_iterZ->end() ) {
                                                ++ ( this->_iterZ );
                                                if ( this->_iterZ == this->_data->end() ) return *this;
                                                this->_iterY = this->_iterZ->begin();
                                        }
                                        this->_iterX = this->_iterY->begin();
                                }
                                return *this;
                        }

                        iterator operator ++ ( int ) {
                                iterator tmp( *this );
                                operator++();
                                return tmp;
                        }

                        bool operator == ( const iterator& rhs ) {
                                return this->is_same( rhs );
                        }

                        bool operator != ( const iterator& rhs ) {
                                return ! this->is_same( rhs );
                        }

                        iterator operator + ( const int n ) {
                                iterator tmp( *this );
                                tmp.operator+=( n );
                                return tmp;
                        }

                        iterator& operator += ( const int n ) {
                                for ( int i = 0 ; i < n ; ++i ) {
                                        ++ ( *this );
                                }
                                return *this;
                        }

                        T& operator*( void ) {
                                return *( this->_iterX );
                        }
                private:
                        bool is_same ( const iterator& rhs ) {
                                if ( this->is_end() ) {
                                        return rhs.is_end();
                                } else {
                                        return ( this->_iterX == rhs._iterX &&
                                                 this->_iterY == rhs._iterY &&
                                                 this->_iterZ == rhs._iterZ );
                                }
                        }
                        bool is_end() const {
                                return ( this->_iterZ == this->_data->end() );
                        }
                };
        private:
                VolumeData( const VolumeData<T>& that );
                void operator = ( const VolumeData<T>& that );
        public:
                explicit VolumeData ( void ) : _isReadable ( false ) {
                        return;
                }

                explicit VolumeData ( const int x, const int y, const int z , const bool allocateMemory = true ) : _isReadable ( false ) {
                        const Point3i size( x,y,z );
                        const VolumeInfo info( size );
                        this->init ( info, allocateMemory );
                        return;
                }

                explicit VolumeData ( const Point3i& size, const bool allocateMemory = true ) : _isReadable ( false ) {
                        const VolumeInfo info( size );
                        this->init ( info, allocateMemory );
                        return;
                }

                explicit VolumeData ( const VolumeInfo& info, const bool allocateMemory = true ) : _isReadable ( false ) {
                        this->init ( info, allocateMemory );
                        return;
                }

                virtual ~VolumeData ( void ) {
                        return;
                }

                VolumeData& init ( const Point3i& size, const bool allocateMemory = true ) {
                        const VolumeInfo info( size );
                        return this->init ( info, allocateMemory );
                }

                VolumeData& init ( const VolumeInfo& info, const bool allocateMemory = true ) {
                        this->deallocate();
                        this->_info.init( info.getSize(), info.getPitch(), info.getOrigin() );
                        if ( allocateMemory ) this->allocate();
                        return *this;
                }

                VolumeData& fill ( const T& value ) {
                        for ( VolumeData::iterator iter = this->begin() ; iter != this->end() ; ++iter ) {
                                *iter = value;
                        }
                        return *this;
                }

                inline VolumeInfo& getInfo ( void ) {
                        return this->_info;
                }

                inline Point3i getSize ( void ) {
                        return this->getInfo().getSize();
                }

                inline T get ( const Point3i& p ) const {
                        return this->at ( p.x(), p.y(), p.z() );
                }

                inline T get ( const int x, const int y, const int z ) const {
                        return this->at( x, y, z );
                }

                inline void set ( const Point3i& p, const T v ) {
                        this->set ( p.x(), p.y(), p.z(), v );
                        return;
                }

                inline void set ( const int x, const int y, const int z , const T v ) {
                        this->at( x, y, z ) = v;
                        return;
                }

                inline T at ( const Point3i& p ) const {
                        return this->at ( p.x(), p.y(), p.z() );
                }

                inline T& at ( const Point3i& p ) {
                        return this->at ( p.x(), p.y(), p.z() );
                }

                inline T at ( const int x, const int y, const int z ) const {
                        return this->_data[z][y][x];
                }

                inline T& at ( const int x, const int y, const int z ) {
                        return this->_data[z][y][x];
                }

                void clear( void ) {
                        const Point3i& size = this->getSize();
                        for( int z = 0 ; z < size.z() ; ++z ) {
                                for( int y = 0 ; y < size.y() ; ++y ) {
                                        for( int x = 0 ; x < size.x() ; ++x ) {
                                                this->set( x,y,z,T() );
                                        }
                                }
                        }
                        return;
                }
                bool clone( VolumeData<T>& that ) {
                        if ( that.getSize() != this->getSize() ) return false;
                        const Point3i& size = this->getSize();
                        for( int z = 0 ; z < size.z() ; ++z ) {
                                for( int y = 0 ; y < size.y() ; ++y ) {
                                        for( int x = 0 ; x < size.x() ; ++x ) {
                                                this->set( x,y,z,that.get( x,y,z ) );
                                        }
                                }
                        }
                        return true;
                }
                bool allocate ( void ) {
                        if ( ! this->isReadable() ) {
                                const Point3i& size = this->_info.getSize ();
                                this->_isReadable = false;
                                this->_data.assign( size.z() , std::vector<std::vector<T> >( size.y(), std::vector<T>( size.x(), T() ) ) );
                                if ( this->_data.size() != static_cast<size_t>( size.z() ) ) return false;
                                this->_isReadable = true;
                        }
                        return true;
                }

                bool deallocate ( void ) {
                        this->_data.erase( this->_data.begin(), this->_data.end() );
                        this->_isReadable = false;
                        return true;
                }

                inline bool isReadable ( void ) const {
                        return this->_isReadable;
                }

                bool check ( void ) {
                        const Point3i& size = this->_info.getSize();
                        const size_t sx = static_cast<size_t>( size.x() );
                        const size_t sy = static_cast<size_t>( size.y() );
                        const size_t sz = static_cast<size_t>( size.z() );
                        if ( this->_data.size() != sz ) return false;
                        for ( int z = 0 ; z < size.z() ; ++z ) {
                                if ( this->_data.at( z ).size() != sy ) return false;
                                for( int y = 0 ; y < size.y() ; ++y ) {
                                        if ( this->_data.at( z ).at( y ).size() != sx ) return false;
                                }
                        }
                        return true;
                }

                iterator begin( void ) {
                        return iterator( &( this->_data ), true );
                }


                iterator end( void ) {
                        return iterator( &( this->_data ), false );
                }

                std::string createFileName( const std::string& name, const std::string ext = std::string( "raw" ) ) {
                        const Point3i& size  = this->getInfo().getSize();
                        const Point3d& pitch = this->getInfo().getPitch();
                        std::stringstream ss;
                        ss<<name<<"-"<<size.x()<<"x"<<size.y()<<"x"<<size.z()<<"-"<<pitch.x()<<"x"<<pitch.y()<<"x"<<pitch.z()<<"."<<ext;
                        return ss.str();
                }
        private:
                VolumeInfo _info;
                std::vector<std::vector<std::vector<T> > > _data;
                bool _isReadable;
        };
};
#endif// MI_VOLUME_DATA_HPP

