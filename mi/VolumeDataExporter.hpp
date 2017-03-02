/**
 * @file VolumeDataExporter.hpp
 * @author Takashi Michikawa <michikawa@acm.org
 */
#ifndef MI_VOLUME_DATA_EXPORTER_HPP
#define MI_VOLUME_DATA_EXPORTER_HPP 1
#include "Exporter.hpp"
#include "VolumeData.hpp"

namespace mi
{
        /**
         * @class VolumeDataExporter VolumeDataExporter.hpp <mi/VolumeDataExporter.hpp>
         * @brief Exporter for VolumeData<T> class.
         */
        template <typename T>
        class VolumeDataExporter : public Exporter
        {
        private:
                VolumeData<T>& _data; ///< Volume data.
        public:
                explicit VolumeDataExporter ( VolumeData<T>& data ) : Exporter ( true ), _data ( data ) {
                        return;
                }
        protected:
                bool writeBody ( std::ofstream& fout ) {
                        if ( ! this->_data.isReadable() ) {
                                std::cerr<<"volume data is not readable."<<std::endl;
                                return false;
                        }

                        const Point3i& size = this->_data.getInfo().getSize();
                        const int numElem = size.x() * size.y();
                        const size_t bufSize = sizeof ( T ) * numElem;

                        std::vector<T> buf ( numElem, T() );
                        for ( int z = 0 ; z < size.z() ; ++z ) {
                                for ( int y = 0 ; y < size.y() ; ++y ) {
                                        for ( int x = 0 ; x < size.x() ; ++x )  {
                                                buf.at ( x + size.x() * y ) = this->_data.at ( x, y, z );
                                        }
                                }
                                if( !fout.write ( ( char* )( &buf[0] ), bufSize ) ) {
                                        std::cerr<<"writing data failed. "<<std::endl;
                                        return false;
                                }
                        }
                        return fout.good();
                }


                std::string toString ( void ) const {
                        return std::string ( "raw" );
                }
        };
};
#endif// MI_VOLUME_DATA_EXPORTER_HPP

