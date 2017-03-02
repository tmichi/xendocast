#ifndef CONSTRAINED_OPENING_ROUTINE_HPP
#define CONSTRAINED_OPENING_ROUTINE_HPP 1
#include <mi/Routine.hpp>
#include <mi/VolumeData.hpp>
#include "ConstrainedMorphology.hpp"

template < typename T = char>
class ConstrainedOpeningRoutine : public mi::Routine
{
private:
        mi::VolumeData<T>& _inData;
        mi::VolumeData<T>& _outData;
        const int _np;
        const int _ng;
public:
        ConstrainedOpeningRoutine ( mi::VolumeData<T>& inData, mi::VolumeData<T>& outData, const int npruning, const int ngrowing )
                : mi::Routine ( "constrained opening" ), _inData( inData ), _outData( outData ), _np( npruning ), _ng( ngrowing )
        {
                if ( !outData.isReadable() ) {
                        this->_outData.init( this->_inData.getInfo() );
                }
                return;
        }

        ~ConstrainedOpeningRoutine ( void )
        {
                return;
        }
private:
        bool main_routine( void )
        {
                ConstrainedMorphology morphology( inData, maskData );
                std::cerr<<"pruning ";
                for ( int i = 0 ; i < this->_np ; ++i ) {
                        morphology.prune();
                        std::cerr<<".";
                }
                std::cerr<<std::endl;
                std::cerr<<"growing ";
                for ( int i = 0 ; i < this->_ng ; ++i ) {
                        morphology.grow();
                        std::cerr<<".";
                }
                std::cerr<<std::endl;
                return true;
        }
};
#endif //CONSTRAINED_OPENING_ROUTINE_HPP
