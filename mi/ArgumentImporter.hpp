/**
 * @file ArgumentImporter.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_ARGUMENT_IMPORTER_HPP
#define MI_ARGUMENT_IMPORTER_HPP 1
#include "Importer.hpp"
#include "Argument.hpp"
#include "Tokenizer.hpp"
namespace mi
{
        /**
         * @class Importer
         * @brief Base class for Impoters.
         */
        class ArgumentImporter : public Importer
        {
        public :
                /**
                 * @brief Constructor.
                 * @param [in] arg The argument object.
                 */
                ArgumentImporter( Argument& arg ) : Importer ( false ), _arg( arg ) {
                        return;
                }
        private:
                /**
                 * @brief Read body part of the file.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                virtual bool readBody ( std::ifstream &fin ) {
                        if ( !fin ) {
                                return false;
                        }
                        while ( fin ) {
                                std::string buffer;
                                std::getline( fin, buffer );
                                if( buffer.length() == 0 ) continue;
                                if( buffer.find_first_of( "#" ) == 0 ) continue;
                                mi::Tokenizer tokenizer( buffer, "#" );
                                mi::Tokenizer tokenizer2( tokenizer.get( 0 ), " " );
                                for ( int i = 0 ; i < tokenizer2.size() ; ++i ) {
                                        this->_arg.add( tokenizer2.get( i ) );
                                }
                        }
                        return true;
                }

                std::string toString ( void ) const {
                        return std::string( "arg" );
                }
        private:
                Argument& _arg;
        };
}
#endif //MI_ARGUMENT_IMPORTER_HPP