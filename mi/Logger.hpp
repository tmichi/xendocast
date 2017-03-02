#ifndef MI_LOGGER_HPP
#define MI_LOGGER_HPP 1
#include <fstream>
#include <sstream>
#include "SystemInfo.hpp"
namespace mi
{
        class Logger
        {
        public:
                static void init ( const std::string& header ) {
                        std::stringstream ss;
                        ss<<header<<"-"<<SystemInfo::getDate()<<".log";
                        Logger::get_instance().open( ss.str().c_str() ) ;
                        if ( ! Logger::get_instance().is_open() ) {
                                std::cerr<<"warning. the log file cannot be open."<<std::endl;
                        } else {
                                std::cerr<<"logs will be saved to "<<ss.str()<<"."<<std::endl;
                        }
                        return;
                }

                static std::ostream& getStream ( void ) {
                        if ( !Logger::get_instance().is_open() ) return std::cerr;
                        else return Logger::get_instance();
                }

        private:
                static std::ofstream& get_instance ( void ) {
                        static std::ofstream fout;
                        return fout;
                }
        };
}
#endif //MI_LOGGER_HPP
