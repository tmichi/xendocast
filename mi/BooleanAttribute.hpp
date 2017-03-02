/**
 * @file BooleanAttribute.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_BOOLEAN_ATTRIBUTE_HPP
#define MI_BOOLEAN_ATTRIBUTE_HPP 1
#include "Attribute.hpp"

namespace mi
{
        /**
         * @class BooleanAttribute BooleanAttribute.hpp "BooleanAttribute.hpp"
         * @brief Attribute for boolean object.
         *
         * @code
         * bool isDebug;
         * mi::AttributeSet attrSet;
         * attrSet.setBooleanAttribute("--debug", isDebug);
         * @endcode
         */
        class BooleanAttribute : public Attribute
        {
        public:
                /**
                 * @brief Constructor.
                 * @param [in] key Key.
                 * @param [in] value Value.
                 */
                explicit BooleanAttribute ( const std::string& key,  bool& value ) : Attribute ( key ) , _value ( value ) {
                        return;
                }

                /**
                 * @brief Analyze the attribute.
                 * @param [in] arg Argument object.
                 * @retval true This method always returns true.
                 */
                bool parse ( const Argument& arg ) {
                        this->_value = arg.exist( this->getKey() );
                        return /* always */ true;
                }
                /**
                 * @brief Returns value.
                 * @return Return "off" when the key is not found and "on" when the key is found.
                 */
                std::string toString( void ) const {
                        std::stringstream ss;
                        ss<<this->getKey()<<" : ";
                        if( this->_value ) ss<<"on";
                        else ss<<"off";
                        return ss.str();
                }
                std::string getValue( void ) const {
                        return ( this->_value ) ? std::string( "on" ) : std::string( "off" );
                }

                BooleanAttribute& setMessage( const std::string& message ) {
                        Attribute::setMessage( message ) ;
                        return *this;
                }

        private:
                bool&    _value; ///< Value.
        }; //class BooleanAttribute
}//namespace mi
#endif // MI_BOOLEAN_ATTRIBUTE_HPP
