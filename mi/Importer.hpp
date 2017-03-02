/**
 * @file Importer.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_IMPORTER_HPP
#define MI_IMPORTER_HPP 1

#include <string>
#include <iostream>
#include <fstream>

namespace mi
{
        /**
         * @class Importer
         * @brief Base class for Impoters.
         */
        class Importer
        {
        private:
                Importer ( const Importer& that );
                void operator = ( const Importer& that );
        protected:
                /**
                 * @brief Constructor.
                 * @param [in] isBinary Open binary mode for reading.
                 */
                explicit Importer ( const bool isBinary = true ) : _isBinary ( isBinary ) {
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                virtual ~Importer ( void ) {
                        return;
                }
                /**
                 * @brief Read header part of the file.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                virtual bool readHeader ( std::ifstream &fin ) {
                        return fin.good();
                }

                /**
                 * @brief Read body part of the file.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                virtual bool readBody ( std::ifstream &fin ) = 0;

                /**
                 * @brief Return importer types.
                 * @return Importer type.
                 */
                virtual std::string toString ( void ) const = 0;

                bool isBinary ( void ) {
                        return this->_isBinary;
                }
        public:
                /**
                 * @brief Read data from a file.
                 * @return true Success.
                 * @return false Failure.
                 */
                bool read ( const std::string& filename  ) {
                        std::ios_base::openmode mode = std::ios_base::in;
                        if( this->_isBinary ) mode |= std::ios::binary;

                        std::ifstream fin ( filename.c_str(), mode );
                        if ( !fin ) {
                                std::cerr<<filename<<" cannot be open."<<std::endl;
                                return false;
                        }

                        std::cerr<<"reading data from "<<filename<<" as "<<this->toString()<<" format ... ";

                        if ( !this->readHeader ( fin ) ) {
                                std::cerr<<"failed. invalid header."<<std::endl;
                                return false;
                        }

                        else if ( !this->readBody ( fin ) ) {
                                std::cerr<<"failed."<<std::endl;
                                return false;
                        }

                        std::cerr<<"done."<<std::endl;
                        return true;
                }
        private:
                const bool _isBinary; ///<Binary mode.
        protected:
                //Status _stat;
        };
};
#endif//MI_IMPORTER_HPP
