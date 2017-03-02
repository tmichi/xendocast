/**
 * @file Parse.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_PARSE_HPP
#define MI_PARSE_HPP 1

#include <string>
#include <cstdlib>
namespace mi
{
        /**
         * @brief Convert string to other types.
         * @param [in] value String.
         * @return Conveted value.
         */
        template<typename T>
        inline T parse ( const std::string &value );

        template<>
        inline char parse ( const std::string &value )
        {
                return static_cast<char> ( std::atoi ( value.c_str() ) );
        }

        template<>
        inline unsigned char parse ( const std::string &value )
        {
                return static_cast<unsigned char> ( std::atoi ( value.c_str() ) );
        }

        template<>
        inline short parse ( const std::string &value )
        {
                return static_cast<short> ( std::atoi ( value.c_str() ) );
        }

        template<>
        inline unsigned short parse ( const std::string &value )
        {
                return static_cast<unsigned short> ( std::atoi ( value.c_str() ) );
        }

        template<>
        inline int parse ( const std::string &value )
        {
                return std::atoi ( value.c_str() );
        }

        template<>
        inline unsigned int parse ( const std::string &value )
        {
                return static_cast<unsigned int> ( std::atoi ( value.c_str() ) );
        }

        template<>
        inline float parse ( const std::string &value )
        {
                return static_cast<float> ( std::atof ( value.c_str() ) );
        }

        template<>
        inline double parse ( const std::string &value )
        {
                return std::atof ( value.c_str() );
        }

        template<>
        inline std::string parse ( const std::string &value )
        {
                return value;
        };
}//namespace mi
#endif// MI_PARSE_HPP
