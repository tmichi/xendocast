#ifndef POROSITY_CLOSING_ROUTINE_HPP
#define POROSITY_CLOSING_ROUTINE_HPP 1
#include <mi/Routine.hpp>
#include <mi/VolumeData.hpp>
#include <mi/VolumeDataUtility.hpp>

template < typename T = char>
class PorosityClosingRoutine : public mi::Routine
{
private:
        mi::VolumeData<T>& _inData;
        mi::VolumeData<T>& _outData;
        const double _radius;
public:
        PorosityClosingRoutine ( mi::VolumeData<T>& inData, mi::VolumeData<T>& outData, const double r )
                : mi::Routine ( "porosity closing" ), _inData( inData ), _outData( outData ), _radius( r )
        {
                if ( !outData.isReadable() ) {
                        this->_outData.init( this->_inData.getInfo() );
                }
                return;
        }

        ~PorosityClosingRoutine ( void )
        {
                return;
        }
private:
        bool main_routine( void )
        {

                return true;
        }
};
#endif //POROSITY_OPENING_ROUTINE_HPP
