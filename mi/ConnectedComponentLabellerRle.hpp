/**
 * @file ConnectedComponentLabellerRle.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_CONNECTED_COMPONENT_LABELLER_RLE_HPP
#define MI_CONNECTED_COMPONENT_LABELLER_RLE_HPP 1
#include <mi/VolumeData.hpp>
#include <mi/RunLengthEncoder.hpp>
#include <cstdlib>
namespace mi
{
        class ConnectedComponentLabellerRle
        {
        private:
                VolumeData<char>& _data;
                std::vector<RunLengthCodeBinary> _codes;
                std::vector<int> _labels;
                std::vector<int> _idx;
                int _num_label;
        public:
                ConnectedComponentLabellerRle( VolumeData<char>& data ) :_data( data ) {
                        return;
                }

                // return the num labels.
                int label ( const bool isSorted = false , const bool joinXyz = true ) {
                        RunLengthEncoder<char> encoder( this->_data );
                        const Point3i& size = this->_data.getInfo().getSize();

                        this->_num_label = encoder.encode( this->_codes, this->_idx );
                        // join in xy-plane
                        for( int z = 0 ; z < size.z() ; ++z ) {
                                for( int y = 0 ; y < size.y() - 1 ; ++y ) {
                                        this->join_xy( z, y );
                                }
                        }

                        if ( joinXyz ) {
                                for( int z = 0 ; z < size.z()-1 ; ++z ) {
                                        this->join_xyz( z );
                                }
                        }

                        this->_labels.assign( this->_codes.size() , 0 );
                        std::vector<int> voxelCount;
                        voxelCount.push_back( 0 );
                        int count = 1;
                        for( int i = 0 ; i < this->_codes.size() ; ++i ) {
                                int parent = this->get_parent( i );
                                if ( parent == i ) {
                                        this->_labels[i] = count;
                                        voxelCount.push_back ( this->_codes[i].getLength() );
                                        ++count;
                                } else {
                                        const int newLabel = this->_labels[ parent ];
                                        this->_labels[i] = newLabel;
                                        voxelCount [ newLabel ] += this->_codes[i].getLength();
                                }
                        }

                        // Sort labels by descend order
                        if ( isSorted ) {
                                std::vector<std::pair<int, int> > pairs;
                                for( int i = 0 ; i < voxelCount.size() ; ++i ) {
                                        pairs.push_back( std::make_pair( - voxelCount[i], i ) );
                                }
                                std::sort( pairs.begin() + 1, pairs.end() );

                                for( int i = 0 ; i < voxelCount.size() ; ++i ) {
                                        pairs[i].first = pairs[i].second;
                                        pairs[i].second = i;
                                }
                                std::sort( pairs.begin() + 1, pairs.end() );

                                for( int i = 0 ; i < this->_codes.size() ; ++i ) {
                                        this->_labels[i] = pairs[ this->_labels[i] ].second;
                                }

                                voxelCount.clear();
                                voxelCount.assign( pairs.size(), 0 ) ;

                                for( int i = 0 ; i < this->_codes.size() ; ++i ) {
                                        voxelCount [ this->_labels[i] ] += this->_codes[i].getLength();
                                }
                        }
                        return count - 1 ;
                }


                template <typename T>
                bool getData( VolumeData<T>& labelData ) {
                        for( int i = 0 ; i < this->_codes.size() ; ++i ) {
                                short int sx, sy, sz, l;
                                this->_codes[i].get( sx,sy,sz, l ) ;
                                for( int j = 0 ; j < l ; ++j ) {
                                        labelData.set( sx+j , sy, sz, this->_labels[i] );
                                }
                        }
                        return true;
                }


                template <typename T>
                bool getNthComponent( VolumeData<T>& labelData , const int n = 1 ) {
                        for( int i = 0 ; i < this->_codes.size() ; ++i ) {
                                if ( n != this->_labels[i] ) continue;
                                short int sx, sy, sz, l;
                                this->_codes[i].get( sx,sy,sz, l ) ;
                                for( int j = 0 ; j < l ; ++j ) {
                                        labelData.set( sx+j , sy, sz, 1 );
                                }
                        }
                        return true;
                }

                /**
                 * label n : 0
                 * others  : 1
                 */
                template <typename T>
                bool getNotNthComponents( VolumeData<T>& labelData , const int n = 1 ) {
                        labelData.fill( 1 );
                        for( int i = 0 ; i < this->_codes.size() ; ++i ) {
                                if ( n != this->_labels[i] ) continue;
                                short int sx, sy, sz, l;
                                this->_codes[i].get( sx,sy,sz, l ) ;
                                for( int j = 0 ; j < l ; ++j ) {
                                        labelData.set( sx+j , sy, sz, 0 );
                                }
                        }
                        return true;
                }

                /* Generalization with function object
                template <typename T>
                bool getPairtialData( VolumeData<T>& labelData , Function fn , Map mp) {
                        for( int i = 0 ; i < this->_codes.size() ; ++i ) {
                                if ( fn( this->_label[i] ) == false) continue;
                		const T value =  mp(this->_label[i]) ;

                		short int sx, sy, sz, l;
                                this->_codes[i].get( sx,sy,sz, l ) ;
                		if ( label != this->_labels[i] ) continue;
                                for( int j = 0 ; j < l ; ++j ) {
                			labelData.set( sx+j , sy, sz, value );
                                }
                        }
                	return true;
                }
                */

        private:
                void join_xyz( const int z ) {
                        const Point3i& size = this->_data.getInfo().getSize();
                        const int id0 =  z * size.y() ;
                        const int id1 =  ( z+1 ) * size.y() ;
                        std::vector<int>& idx = this->_idx;

                        for( int i = 0 ; i < size.y() ; ++i ) {
                                for( int j =  -1 ;  j <= 1 ; ++j ) {
                                        if ( i + j < 0 ) continue;
                                        if ( i + j >= size.y() ) continue;
                                        for( int k = idx [ id0 + i ] ; k < idx[id0+ i + 1] ; ++k ) {
                                                for( int l = idx [ id1 + i + j ] ; l < idx[id1 + i + j +1] ; ++l ) {
                                                        this->connect( k, l );
                                                }
                                        }
                                }
                        }
                }


                void join_xy( const int z , const int y ) {
                        const Point3i& size = this->_data.getInfo().getSize();
                        const int id0 =  z * size.y() + y ;
                        std::vector<int>& idx = this->_idx;
                        for( int i = idx[ id0 ] ; i < idx[id0+1] ; ++i ) {
                                for ( int j = idx[id0+1] ; j <idx[id0+2] ; ++j ) {
                                        this->connect( i, j );
                                }
                        }
                }

                int get_parent( int id ) {
                        const int parent = this->_codes[id].getParent();
                        if ( parent == -1 ) return id;
                        else return this->get_parent( parent );
                }

                void set_parent ( int p, int c ) {
                        this->_codes[c].setParent( p );
                        this->_num_label -= 1;
                }


                void connect ( const int i , const int j ) {
                        if ( this->_codes[i].isConnected( this->_codes[j] ) ) {
                                const int parent0 = this->get_parent( i );
                                const int parent1 = this->get_parent( j );

                                if ( parent0 == -1 && parent1 == -1 ) {
                                        this->set_parent( i, j );
                                } else if ( parent0 != -1 && parent1 != -1 )  {
                                        if( parent0 < parent1 ) {
                                                this->set_parent ( parent0, parent1 );
                                        } else if ( parent0 > parent1 ) {
                                                this->set_parent ( parent1, parent0 );
                                        }
                                } else {
                                        if ( parent0 == -1 ) this->set_parent( parent1, i );
                                        if ( parent1 == -1 ) this->set_parent( parent0, j );
                                }
                        }
                }
        };
}
#endif //MI_CONNECTED_COMPONENT_LABELLER_RLE_HPP
