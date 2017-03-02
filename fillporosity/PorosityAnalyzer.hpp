#ifndef POROSITY_ANALYZER_HPP
#define POROSITY_ANALYZER_HPP 1

#include <mi/VolumeData.hpp>

#define POROSITY_BG    0x01
#define POROSITY_FG    0x02
#define POROSITY_OPEN  0x04
#define POROSITY_CLOSED 0x08

class PorosityAnalyzer
{
private:
        const mi::VolumeData<char>& _data; // binary image
        mi::VolumeData<char> _porosity;
        double _radius;
        int _num_pruning;
        int _num_growing;

public:
        PorosityAnalyzer ( const mi::VolumeData<char>& data,  const double radius, const int num_pruning, const int num_growing );
        ~PorosityAnalyzer ( void ) ;

        int analyze ( void );

        int getPorosity ( mi::VolumeData<char>& voidData , unsigned char detected_types , bool isBinarized = false ) ;

        template<typename T>
        int getReplacedVolume ( mi::VolumeData<T>& data, const T value );

private:
        int close_porosity ( const mi::VolumeData<char>& inData, mi::VolumeData<char>& outData, const double radius );
        int remove_false_porosity ( mi::VolumeData<char>& inData, mi::VolumeData<char>& outData, const int np, const int ng );

        int classify_porosity ( const mi::VolumeData<char>& inData, const mi::VolumeData<char>& maskData, mi::VolumeData<char>& outData );
};
#endif // POROSITY_ANALYZER_HPP

