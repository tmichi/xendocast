#ifndef MI_RUN_LENGTH_ENCODER_HPP
#define MI_RUN_LENGTH_ENCODER_HPP 1
#include <mi/VolumeData.hpp>
#include <mi/RunLengthCodeBinary.hpp>
namespace mi
{
        template<typename T>
        class RunLengthEncoder
        {
        private:
                const VolumeData<T>& _data;
        public:
                RunLengthEncoder ( const VolumeData<T>& data ) : _data( data ) {
                        return;
                }
		
                int encode ( std::vector< RunLengthCodeBinary >& codes , std::vector<int>& idx ) {
                        int count = 0;
                        const mi::Point3i& size = const_cast< VolumeData<T>& >( this->_data ).getInfo().getSize();
                        idx.reserve( size.y() * size.z() );
                        for( short int z = 0 ; z < size.z() ; ++z ) {
                                for( short int y = 0 ; y < size.y() ; ++y ) {
                                        idx.push_back( count );
                                        short int start = 0;
                                        char value0 = this->_data.get( 0,y,z );
                                        short int length = 1;

                                        for( int x = 1 ; x < size.x() ; ++x ) {
                                                T value1 = this->_data.get( x,y,z );
                                                if ( value0 == value1 ) {
                                                        ++length;
                                                } else {
                                                        if ( value0 == 1 ) {
                                                                RunLengthCodeBinary code( start, y, z, length );
                                                                codes.push_back( code );
                                                                ++count;
                                                        }
                                                        start = x;
                                                        value0 = value1;
                                                        length = 1;
                                                }
                                        }
					
                                        if ( value0 == 1 ) {
                                                RunLengthCodeBinary code1( start, y, z, length );
                                                codes.push_back( code1 );
                                                ++count;
                                        }
                                }
                        }
                        idx.push_back( count );
                        return count;
                }
        };
}
#endif //MI_RUN_LENGTH_ENCODER_HPP
