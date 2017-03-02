/**
 * @file Exporter.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_EXPORTER_HPP
#define MI_EXPORTER_HPP 1

#include <string>
#include <iostream>
#include <fstream>
namespace mi
{
        /**
         * @class Exporter Exporter.hpp "Exporter.hpp"
         * @brief Base class for Exporters.
         */
        class Exporter
        {
        private:
                Exporter ( const Exporter& that );
                void operator = ( const Exporter& that );
        protected:
                /**
                 * @brief Constructor.
                 * @param [in] isBinary binary mode flag.
                 */
                explicit Exporter ( const bool isBinary = true ) : _isBinary( isBinary ) {
                        return;
                }

                /**
                 * @brief Destructor.
                 */
                virtual ~Exporter ( void ) {
                        return;
                }
                /**
                 * @brief Write header part of the file.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                virtual bool writeHeader( std::ofstream &fout ) {
                        return fout.good();
                }
                /**
                 * @brief Write body part of the file.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                virtual bool writeBody ( std::ofstream &fout ) = 0;

                /**
                 * @brief Check data mode.
                 * @retval true Binary mode.
                 * @retval false Ascii mode.
                 */
                bool isBinary( void ) const {
                        return  this->_isBinary;
                }
        public:
                /**
                 * @brief Return exporter types.
                 * @return Exporter type.
                 */
                virtual std::string toString ( void ) const = 0;

                /**
                 * @brief Write data to a file.
                 * @return true Success.
                 * @return false Failure.
                 */
                bool write ( const std::string& filename ) {
                        std::ios_base::openmode mode = std::ios_base::out;
                        if( this->_isBinary ) mode |= std::ios::binary;

                        std::ofstream fout ( filename.c_str(), mode );
                        if( !fout ) {
                                std::cerr<<filename<<"is not found."<<std::endl;
                                return false;
                        }

                        std::cerr<<"writing data to "<<filename<<" as "<<this->toString()<<" format ... ";
                        if ( !this->writeHeader ( fout ) ) {
                                std::cerr<<"failed."<<std::endl
                                         <<"file header was not written."<<std::endl;
                                return false;
                        }

                        if ( !this->writeBody ( fout ) ) {
                                std::cerr<<"failed."<<std::endl
                                         <<"file body was not written."<<std::endl;
                                return false;
                        }
                        std::cerr<<"done."<<std::endl;
                        return true;
                }
        private:
                const bool _isBinary; ///< Binary mode.
        };
}
#endif //MI_EXPORTER_HPP
