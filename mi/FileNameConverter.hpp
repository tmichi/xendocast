/**
 * @file FileNameConverter.hpp
 * @author Takashi Michikawa
 */
#ifndef MI_FILE_NAME_CONVERTER_HPP
#define MI_FILE_NAME_CONVERTER_HPP 1

#include <cctype>
#include <sstream>
#include <iomanip>

namespace mi
{
        /**
         * @class FileNameConverter FileNameConverter.hpp "FileNameConverter.hpp"
         * @brief Converter for changing filename.
         */
        class FileNameConverter
        {
        private:
                FileNameConverter ( const FileNameConverter& that );
                void operator = ( const FileNameConverter& that );
        public:
                /**
                 * @brief Contrucor.
                 * @param [in] filename File name.
                 */
                explicit FileNameConverter ( const std::string filename = std::string() ) : _filename( filename ) {
                        return;
                }

                /**
                 * @brief Destructor.
                 */
                ~FileNameConverter ( void ) {
                        return;
                }

                /**
                 * @brief Remove extension (".raw", ".stl" and so on ) from the filename.
                 * @return File name without extension.
                 */
                std::string removeExtension ( void ) const {
                        std::stringstream ss;
                        ss<<this->getOriginal().substr ( 0, this->getOriginal().find_last_of ( "." ) );
                        return ss.str();
                }

                /**
                 * @brief Add new extenstion to the filename.
                 * @param [in] ext new extension.
                 * @return File name with new extension.
                 */
                std::string getNewFileName ( const std::string& ext ) {
                        std::stringstream ss;
                        ss<<this->removeExtension() <<"."<<ext;
                        return ss.str();
                }

                /**
                 * @brief Add index and new extenstion to the filename.
                 * @param [in] ext new extension.
                 * @param [in] index new index.
                 * @param [in] fillN minimum order of index. If order of index is less than fillN, "0" is filled.
                 * @return File name with index and new extension.
                 */
                std::string getNewFileName ( const std::string& ext, const int index , const int fillN = 0 ) {
                        std::stringstream ss;
                        ss<<this->removeExtension() <<"-"<<std::setw( fillN )<<std::setfill( '0' )<<index<<"."<<ext;
                        return ss.str();
                }

                /**
                 * @brief Get extension of the file.
                 * @retval Extension of the file.
                 */
                std::string getExtension ( void ) const {
                        return this->_filename.substr ( this->_filename.find_last_of ( "." ) + 1 );
                }

                /**
                 * @brief Check the filename has the extension.
                 * @param [in] ext Extension.
                 * @retval true The file name has the extension.
                 * @retval true The file name does not hav the extension.
                 */
                bool checkExtension ( const std::string ext ) {
                        const std::string src = this->getExtension();
                        const std::string src_upper = this->to_upper ( src );
                        const std::string ext_upper = this->to_upper ( ext );
                        return src_upper.compare ( ext_upper ) == 0;
                }

                /**
                 * @brief Get the original file name.
                 * @return File name.
                 */
                std::string getOriginal ( void ) const {
                        return this->_filename;
                }
        private:
                /**
                 * @brief Capitalize the string.
                 * @param [in] str String.
                 @ return Capitalized string.
                 */
                std::string to_upper ( const  std::string str ) const {
                        std::string result = str;
                        for ( std::string::iterator iter = result.begin() ; iter != result.end() ; iter++ ) {
                                *iter = static_cast<char> ( std::toupper ( *iter ) );
                        }
                        return result;
                }
        private:
                std::string _filename; ///> File name.
        };
};
#endif// MI_FILE_NAME_CONVERTER_HPP
