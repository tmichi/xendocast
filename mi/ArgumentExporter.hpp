#ifndef MI_ARGUMENT_EXPORTER_HPP
#define MI_ARGUMENT_EXPORTER_HPP 1
#pragma once
#include "Exporter.hpp"
#include "Argument.hpp"
namespace mi
{
        /**
         * @class Exporter
         * @brief Base class for Impoters.
         */
        class ArgumentExporter : public Exporter
        {
        public :
                ArgumentExporter( Argument& arg ) : Exporter ( false ), _arg( arg ) {
                        return;
                }
        private:
                virtual bool writeBody ( std::ofstream &fout ) {
                        for( int i = 0 ; i < this->_arg.size() ; ++i ) {
                                const std::string& str = this->_arg.get<std::string>( i );
                                if ( str.find( "-" ) != std::string::npos ) fout<<std::endl;
                                else fout<<" ";
                                fout<<std::endl;
                        }
                        fout<<std::endl;
                        return true;
                }

                std::string toString ( void ) const {
                        return std::string( "arg" );
                }
        private:
                Argument& _arg;
        };
}
#endif //MI_ARGUMENT_EXPORTER_HPP
