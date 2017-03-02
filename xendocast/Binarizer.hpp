#ifndef BINARIZER_HPP
#define BINARIZER_HPP 1
#include <mi/VolumeData.hpp>
template<typename S, typename T = char>
class Binarizer
{
private:
        const mi::VolumeData<S>& _data;
        T _fgValue;
        T _bgValue;
public:
        Binarizer ( const mi::VolumeData<S>& data, const T fg = 1, const T bg = 0 ): _data( data ), _fgValue ( fg ), _bgValue( bg )
        {
                return;
        }

        void binarize ( const S isovalue, mi::VolumeData<T>& binaryData )
        {
                const mi::VolumeInfo& info = const_cast<mi::VolumeData<S>&>( this->_data ).getInfo();
                const mi::Point3i& size = info.getSize();
                binaryData.init( info );
                for( int z = 0 ; z < size.z() ; ++z ) {
                        for( int y = 0 ; y < size.y() ; ++y ) {
                                for( int x = 0 ; x < size.x() ; ++x ) {
                                        const T value = ( this->_data.get( x,y,z ) < isovalue ) ?  this->_bgValue : this->_fgValue;
                                        binaryData.set( x,y,z, value ) ;
                                }
                        }
                }
        }

        void binarize ( const mi::VolumeData<S>& isovalueField, mi::VolumeData<T>& binaryData )
        {
                const mi::Point3i &size = this->_data.getSize();
                binaryData.init( this->_data.getInfo() );
                for( int z = 0 ; z < size.z() ; ++z ) {
                        for( int y = 0 ; y < size.y() ; ++y ) {
                                for( int x = 0 ; x < size.x() ; ++x ) {
                                        const T value = ( this->_data.get( x,y,z ) < isovalueField.get( x,y,z ) ) ?  this->_bgValue : this->_fgValue;
                                        binaryData.set( x,y,z, value ) ;
                                }
                        }
                }
        }
};

#endif //BINARIZER_HPP
