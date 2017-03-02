/**
 * @file NumericAttribute.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_NUMERIC_ATTRIBUTE_HPP
#define MI_NUMERIC_ATTRIBUTE_HPP 1
#include "Attribute.hpp"
namespace mi
{
        /**
         * @class NumericAttribute  NumericAttribute.hpp "NumericAttribute.hpp"
         * @brief Attribute for numeric object.
         * @code
         * int value;
         * mi::AttributeSet attrSet;
         * attrSet.setNumericAttribute<int>("-i", value).setMandatory().setMinMax(10, 100);
         * @endcode
         */
        template <typename T>
        class NumericAttribute : public Attribute
        {
        public:
                /**
                 * @brief Constructor.
                 * @param [in] key Key.
                 * @param [in] value Numeric object.
                 * @param [in] offset Offset value.
                 */
                explicit NumericAttribute ( const std::string& key,  T& value, const int offset = 1 )
                        : Attribute ( key ) , _value ( value ), _offset ( offset ),
                          _minValue( T() ), _maxValue( T() ), _defaultValue( T() ),
                          _isMinSet( false ), _isMaxSet( false ), _isOutRangeRejected( false ) {
                        return;
                }

                /**
                 * @brief Parse attribute.
                 * @param[in] arg Argument object.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                bool parse ( const Argument& arg ) {
                        if ( this->is_key_found( arg, this->getKey(), this->_offset ) ) {
                                this->_value = arg.get<T>( this->getKey() , this->_offset );
                                return this->clamp_value( this->_value );
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
                 * @brief Set minimum value.
                 * @param [in] minValue Minimum value.
                 * @return Itself.
                 */
                NumericAttribute<T>&  setMin( const T minValue ) {
                        this->_minValue = minValue;
                        this->_isMinSet = true;

                        if ( this->_isMaxSet ) {
                                this->swap_min_max( this->_minValue, this->_maxValue );
                        }

                        this->clamp_value( this->_defaultValue );
                        return *this;
                }

                /**
                 * @brief Set maximum value.
                 * @param [in] maxValue Maximum value.
                 * @return Itself.
                 */
                NumericAttribute<T>& setMax( const T maxValue ) {
                        this->_maxValue = maxValue;
                        this->_isMaxSet = true;
                        if ( this->_isMinSet ) {
                                this->swap_min_max( this->_minValue, this->_maxValue );
                        }
                        this->clamp_value( this->_defaultValue );
                        return *this;
                }

                /**
                 * @brief Set minimum / maximum value.
                 * @param [in] minValue Minimum value.
                 * @param [in] maxValue Maximum value.
                 * @return Itself.
                 */
                NumericAttribute<T>& setMinMax( const T minValue, const T maxValue ) {
                        return this->setMin ( minValue ).setMax ( maxValue );
                }

                /**
                 * @brief Set default value.
                 * @param [in] defaultValue Default value.
                 * @return Itself.
                 */
                NumericAttribute<T>&  setDefaultValue ( const T defaultValue ) {
                        this->_defaultValue = defaultValue;
                        this->clamp_value( this->_defaultValue );
                        return *this;
                }

                /**
                 * @brief Set this attribute must be within a range.
                 * @return Itself.
                 */
                NumericAttribute<T>& setOutRangeRejected( void ) {
                        this->_isOutRangeRejected = true;
                        return *this;
                }

                /**
                 * @brief Get value.
                 * @return Value.
                 */
                std::string toString( void ) const {
                        std::stringstream ss;
                        //ss<<this->getKey()<<"["<<this->_offset<<"] : "<<this->getValue();
                        ss<<this->getKey();
                        if ( this->_offset > 1 ) ss<<"["<<this->_offset<<"]";
                        ss<<" : "<<this->getValue();

                        return ss.str();
                }

                std::string getValue( void ) const {
                        std::stringstream ss;
                        ss<<this->_value;
                        return ss.str();
                }

                /**
                 * @brief Set this attribute is mandatory.
                 * @return Itself.
                 */
                NumericAttribute<T>& setMandatory( void ) {
                        Attribute::setMandatory();
                        return *this;
                }
                NumericAttribute<T>& setHidden ( void ) {
                        Attribute::setHidden();
                        return *this;
                }
                NumericAttribute<T>& setMessage( const std::string& message ) {
                        Attribute::setMessage( message ) ;
                        return *this;
                }

        private:
                bool clamp_value ( T& value ) {
                        if ( this->_isMinSet && value < this->_minValue ) {
                                if( this->_isOutRangeRejected ) {
                                        this->setErrorCode( ATTRIBUTE_ERROR_VALUE_OUT_OF_RANGE );
                                        return false;
                                }
                                value = this->_minValue;
                        }
                        if ( this->_isMaxSet && value > this->_maxValue ) {
                                if( this->_isOutRangeRejected ) {
                                        this->setErrorCode( ATTRIBUTE_ERROR_VALUE_OUT_OF_RANGE );
                                        return false;
                                }
                                value = this->_maxValue;
                        }
                        return true;
                }


                void swap_min_max ( T& minValue, T& maxValue ) {
                        if( maxValue <  minValue ) {
                                const T tmp = minValue;
                                minValue = maxValue;
                                maxValue = tmp;
                        }
                        return;
                }
        private:
                T&    _value; ///< Value.

                int _offset; ///< Offset

                T    _minValue; ///< Minimum value.
                T    _maxValue; ///< Maximum value.
                T    _defaultValue;  ///< Default value.

                bool _isMinSet; ///< Minimum value is set.
                bool _isMaxSet; ///< Maximum value is set.
                bool _isOutRangeRejected; ///< Out rangevalue is rejected.
        };
};
#endif// MI_NUMERIC_ATTRIBUTE_HPP
