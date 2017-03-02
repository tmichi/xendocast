#ifndef CONSTAINED_MORPHOLOGY_HPP
#define CONSTAINED_MORPHOLOGY_HPP 1
#include <mi/VolumeData.hpp>
#include <mi/Neighbor.hpp>
class ConstrainedMorphology
{
private:
        mi::VolumeData<char>& _data;
        const mi::VolumeData<char>& _mask;

public:
        ConstrainedMorphology ( mi::VolumeData<char>& data, const mi::VolumeData<char>& mask );
        ~ConstrainedMorphology ( void );
        bool prune ( void );
        bool grow ( void );
private:
        bool morphology ( const char fgValue, const char bgValue );

        class morphology_fn
        {
        private:
                const  mi::VolumeData<char>& _srcData;
                const  mi::VolumeData<char>& _maskData;
                mi::VolumeData<char>& _trgData;
                char _fgValue;
                char _bgValue;
        public :
                morphology_fn( const mi::VolumeData<char>& srcData, const mi::VolumeData<char>& maskData, mi::VolumeData<char>& trgData, char fgValue, char bgValue ) : _srcData( srcData ), _maskData( maskData ), _trgData( trgData ), _fgValue( fgValue ), _bgValue( bgValue )
                {
                        return;
                }
                void operator () ( const mi::Point3i& p )
                {
                        char result = this->_srcData.get( p );
                        if ( result == this->_bgValue ) {
                                for ( mi::Neighbor::iterator diter = mi::Neighbor::begin() ; diter != mi::Neighbor::end6() ; ++diter ) {
                                        const mi::Point3i np = p + *diter;
                                        if ( ! this->_trgData.getInfo().isValid( np ) ) continue;
                                        if ( this->_maskData.get( np ) == 1 ) continue;
                                        if ( this->_srcData.get( np ) == this->_fgValue ) result = this->_fgValue;
                                }
                        }
                        this->_trgData.set( p, result );
                }
        };
};

#endif//CONSTAINED_MORPHOLOGY_HPP
