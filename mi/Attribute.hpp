/**
 * @file Attribute.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_ATTRIBUTE_HPP
#define MI_ATTRIBUTE_HPP 1
#include <string>
#include <sstream>
#include <iostream>

#include "Argument.hpp"
namespace mi
{
        /**
         * @enum AttributeErrorCode
         * @brief Status of parsing.
         */
        enum AttributeErrorCode {
                ATTRIBUTE_ERROR_OK                 = 0,   ///< OK.
                ATTRIBUTE_ERROR_KEY_NOT_FOUND      = -1,  ///< The key cannot be found.
                ATTRIBUTE_ERROR_VALUE_OUT_OF_RANGE = -2,  ///< The value is out of range.
                ATTRIBUTE_ERROR_INVALID            = -100 ///< Other error.
        };

        /**
         * @class Attribute Attribute.hpp <mi/Attribute.hpp>
         * @brief Base class for attributes.
         * @sa AttributeSet, NumericAttribute, BooleanAttribute, StringAttribute
         */
        class Attribute
        {
        private:
                Attribute ( const Attribute& that );
                void operator = ( const Attribute& that );
        protected:
                /**
                 * @brief Constructor.
                 * @param [in] key Key.
                 */
                explicit Attribute ( const std::string& key ) : _key( key ), _isMandatory( false ), _isHidden( false ), _errorCode( ATTRIBUTE_ERROR_OK ) {
                        return ;
                }
        public:
                /**
                 * @brief Destructor.
                 */
                virtual ~Attribute( void ) {
                        return;
                }

                /**
                 * @brief Set this attribute mandatory.
                 *
                 * When this method is called and the key is not found, Attribute::getErrorCode() returns ATTRIBUTE_ERROR_KEY_NOT_FOUND.
                 * @return Reference to itself.
                 */
                virtual Attribute& setMandatory( void ) {
                        this->_isMandatory = true;
                        return *this;
                }

                virtual Attribute& setHidden ( void ) {
                        this->_isHidden = true;
                        return *this;
                }

                /**
                 * @brief Analyze the attribute.
                 * @param [in] arg Argument object.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                virtual bool parse ( const Argument& arg ) = 0;

                /**
                 * @brief Return value.
                 * @retval Value.
                 */
                virtual std::string toString( void ) const = 0;

                /**
                 * @brief Print error message if the error exists.
                 */
                virtual void printError ( void ) {
                        switch ( this->getErrorCode() ) {
                        case ATTRIBUTE_ERROR_OK :
                                break;
                        case ATTRIBUTE_ERROR_KEY_NOT_FOUND :
                                std::cerr<<this->getKey()<<" was not found."<<std::endl;
                                break;
                        case ATTRIBUTE_ERROR_VALUE_OUT_OF_RANGE :
                                std::cerr<<"key value "<<this->getValue()<<" (for "<<this->getKey()<<") is out-of-range."<<std::endl;
                                break;
                        default : //  ATTRIBUTE_ERROR_INVALID
                                std::cerr<<"unknown error found."<<std::endl;
                                break;
                        }
                        return;
                }

                /**
                 * @brief Print the key and associated value.
                 */
                virtual void print( std::ostream& out = std::cerr ) {
                        out<<this->toString()<<std::endl;
                }

                /**
                 * @brief Get key string.
                 * @return Key.
                 */
                std::string getKey( void ) const {
                        return this->_key;
                }

                virtual std::string getValue ( void ) const {
                        return std::string();
                }
                /**
                 * @brief Return the attribute is mandatory or not.
                 * @retval true The attribute is mandatory.
                 * @retval false The attribute is not mandatory.
                 */
                bool isMandatory ( void ) const {
                        return this->_isMandatory;
                }
        protected:
                /**
                 * @brief Check the key exists or not.
                 * @param [in] arg Argument object.
                 * @param [in] key Key.
                 * @param [in] offset Offset.
                 * @retval true Value exist.
                 * @retval false  Value does  not exist.
                 */
                bool is_key_found( const Argument& arg, const std::string& key, int offset = 0 ) {
                        return arg.exist( key, offset );
                }

                /**
                 * @brief Get error code.
                 * @return Error code.
                 * @sa AttributeErrorCode
                 */
                AttributeErrorCode getErrorCode ( void  ) const {
                        return this->_errorCode;
                }
                /**
                 * @brief Set error code.
                 * @param [in] code Error code.
                 */
                void setErrorCode( const AttributeErrorCode code ) {
                        this->_errorCode = code;
                        return;
                }


        public:
                void setMessage( const std::string& message ) {
                        this->_message = message;
                        return;
                }

                std::string getMessage ( void ) const {
                        return this->_message;
                }

                virtual void print_usage ( void ) {
                        if ( this->_isHidden ) return;
                        std::cerr<<"\t"<<this->getKey()<<"\t"<<this->getMessage()<<std::endl;
                }

        private:
                std::string        _key;         ///< Key string.
                std::string        _message;
                bool               _isMandatory; ///< Flag this is mandatory attribute.
                bool               _isHidden; ///< This is a hidden attribute.
                AttributeErrorCode _errorCode;   ///< Error code
        };
}
#endif //MI_ATTRIBUTE_HPP
