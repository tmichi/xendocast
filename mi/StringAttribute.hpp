/**
 * @file StringAttribute.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_STRING_ATTRIBUTE_HPP
#define MI_STRING_ATTRIBUTE_HPP 1
#include "Attribute.hpp"
namespace mi
{
        /**
         * @class StringAttribute StringAttribute.hpp <mi/StringAttribute.hpp>
         * @brief Manage string type attributes.
         */
        class StringAttribute : public Attribute
        {
        public:
                /**
                 * @brief Constructor.
                 * @param [in] key Key.
                 * @param [in] value Value.
                 */
                explicit StringAttribute ( const std::string& key,  std::string& value ) : Attribute ( key ) , _value ( value ) {
                        return;
                }

                /**
                 * @brief Destructor.
                 */
                ~StringAttribute ( void ) {
                        return;
                }
                /**
                 * @brief Parse the argument.
                 * @param [in] arg Argument.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                bool parse ( const Argument& arg ) {
                        if ( this->is_key_found( arg, this->getKey(), 1 ) ) {
                                this->_value = arg.get<std::string>( this->getKey() );
                                return true;
                        } else {
                                if ( this->isMandatory() ) {
                                        this->setErrorCode( ATTRIBUTE_ERROR_KEY_NOT_FOUND );
                                        return false;
                                } else {
                                        this->_value = this->_defaultValue;
                                        return true;
                                }
                        }
                }
                /**
                 * @brief Set default value.
                 * @return A reference to the object.
                 */
                StringAttribute&  setDefaultValue ( const std::string& defaultValue ) {
                        this->_defaultValue = defaultValue;
                        return *this;
                }
                /**
                 * @brief Set this attribute is mandatory.
                 * @return A reference to the object.
                 */
                StringAttribute& setMandatory( void ) {
                        Attribute::setMandatory();
                        return *this;
                }
                StringAttribute& setHidden ( void ) {
                        Attribute::setHidden();
                        return *this;
                }

                StringAttribute& setMessage( const std::string& message ) {
                        Attribute::setMessage( message ) ;
                        return *this;
                }

                /**
                 * @brief Get string value.
                 * @return String value.
                 */
                std::string toString( void ) const {
                        std::stringstream ss;
                        ss<<this->getKey()<<" : "<<this->_value;
                        return ss.str();
                }

                std::string getValue ( void ) const {
                        return this->_value;
                }
        private:
                std::string&    _value; ///< Value.
                std::string     _defaultValue; ///< Default value.
        };
};
#endif// MI_STRING_ATTRIBUTE_HPP
