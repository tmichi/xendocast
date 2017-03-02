/**
 * @file VolumeDataImporter.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_VOLUME_DATA_IMPORTER_HPP
#define MI_VOLUME_DATA_IMPORTER_HPP 1
#include <string>
#include "VolumeData.hpp"
#include "Importer.hpp"
namespace mi
{
        /**
         * @class VolumeDataImporter
         * @brief Importer for raw file.
         */
        template <typename T>
        class VolumeDataImporter : public Importer
        {
        private:
                VolumeData<T>& _data; ///< Data.
                const size_t   _header_size; ///< Header size ( byte ).
        public:
                /**
                 * @brief Constructor.
                 * @param [out] data Data.
                 * @param [in] header_size Header size (byte).
                 */
                explicit VolumeDataImporter ( VolumeData<T>& data, const size_t header_size = 0 )
                        : Importer ( true ), _data ( data ), _header_size ( header_size ) {
                        return;
                }
        protected:
                /**
                 * @brief Read header part.
                 * @param [in] Input stream.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                virtual bool readHeader ( std::ifstream& fin ) {
                        fin.seekg( this->_header_size ).good();
                        return fin.good();
                }
                /**
                 * @brief Read dody part.
                 * @param [in] Input stream.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                bool readBody ( std::ifstream& fin ) {
                        if ( !this->_data.isReadable() ) {
                                std::cerr<<"error : volume data cannot be read. memory space is not allocated yet."<<std::endl;
                                return false;
                        }

                        const Point3i& size = this->_data.getInfo().getSize ();
                        std::vector<T> buffer ( size.x() * size.y(), T() );
                        const size_t bufSize = sizeof ( T ) * buffer.size();

                        for ( int z = 0 ; z < size.z() ; ++z ) {
                                if( !fin.read ( ( char* ) & ( buffer[0] ), bufSize ) ) {
					std::cerr<<"size"<<sizeof (T)<<std::endl;
                                        std::cerr<<"reading data failed."<<z<<"/"<<size.z()<<std::endl;
                                        return false;
                                }
                                for ( int y = 0 ; y < size.y() ; ++y ) {
                                        for ( int x = 0 ; x < size.x() ; ++x ) {
                                                this->_data.at ( x, y, z ) = buffer[ x +  y * size.x() ];
                                        }
                                }
                        }
                        return true;
                }
                /**
                 * @brief Get volume data reference.
                 * @retval Volume data reference.
                 */
                mi::VolumeData<T>& getVolume ( void ) {
                        return this->_data;
                }
                /**
                 * @brief Get string.
                 * @retval String.
                 */
                std::string toString ( void ) const {
                        return std::string ( "raw" );
                }

        };
}
#endif// MI_VOLUME_DATA_IMPORTER_HPP
