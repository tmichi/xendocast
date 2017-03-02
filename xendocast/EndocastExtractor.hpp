#ifndef ENDOCAST_EXTRACTOR_HPP
#define ENDOCAST_EXTRACTOR_HPP 1
#include <mi/VolumeData.hpp>

class EndocastExtractor
{
private:
public:
        EndocastExtractor ( void ) ;
        ~EndocastExtractor ( void ) ;
        bool extract ( mi::VolumeData<float>& distData, mi::VolumeData<int>& labelData, const double scale ) ;

private:
        bool init ( mi::VolumeData<float>& distData, mi::VolumeData<int>& labelData, const double scale ) ;
        bool run ( mi::VolumeData<float>& distData, mi::VolumeData<int>& labelData ) ;
};
#endif // ENDOCAST_EXTRACTOR_HPP