/**
 * @file diff.hpp
 * @author Takashi Michikawa <michi@den.rcast.u-tokyo.ac.jp>
 */
#ifndef MI_FUNCTIONAL_DIFF_HPP
#define MI_FUNCTIONAL_DIFF_HPP 1
#include <functional>
#include <mi/VolumeData.hpp>
#include <mi/math.hpp>
namespace mi
{
        class diff
        {
        private:
                const VolumeData<char>& _srcData;
                const VolumeData<char>& _trgData;
                VolumeData<char>& _outData;
        public:
                diff ( const  VolumeData<char>& srcData, const VolumeData<char>& trgData, VolumeData<char>& outData ) :
                        _srcData( srcData ), _trgData( trgData ), _outData( outData ) {

                        return;
                }

                void operator () ( const Point3i& p ) {
                        char value = 0;
                        if ( this->_srcData.get( p ) == 1  && this->_trgData.get( p ) == 0 ) value = 1;
                        this->_outData.set( p, value );
                        return;
                }
        };
};
#endif //MI_FUNCTIONAL_DILATE_HPP
