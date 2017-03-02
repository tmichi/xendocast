#ifndef BINARIZATION_ROUTINE_HPP
#define BINARIZATION_ROUTINE_HPP 1
#include "Routine.hpp"
#include <mi/VolumeData.hpp>

template<typename T, typename S = char>
class BinarizationRoutine : public mi::Routine
{
private:
        mi::VolumeData<T>& _inData;
        mi::VolumeData<S>& _outData;
        double _threshold;
public:
        BinarizationRoutine ( mi::VolumeData<T>& inData, mi::VolumeData<S>& outData, const T threshold ) : mi::Routine ( "binarization" ), _inData( inData ), _outData( outData ), _threshold( threshold )
        {
                if ( !outData.isReadable() ) {
                        this->_outData.init( this->_inData.getInfo() );
                }
                return;
        }

        ~BinarizationRoutine ( void )
        {
                return;
        }
private:
        bool main_routine( void )
        {
                const mi::Point3i& size = this->_inData.getInfo().getSize();
                for( int z = 0 ; z < size.z() ; ++z ) {
                        for ( int y = 0 ; y < size.y() ; ++y ) {
                                for( int x = 0; x < size.x() ; ++x ) {
                                        if ( this->_inData.get( x,y,z ) < this->_threshold ) this->_outData.set( x,y,z, 0 );
                                        else                                                 this->_outData.set( x,y,z, 1 );
                                }
                        }
                }
                return true;
        }
};
#endif //BINARIZATION_ROUTINE_HPP
