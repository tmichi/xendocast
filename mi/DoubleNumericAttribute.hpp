/**
 * @file DoubleNumericAttribute.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_DOUBLE_NUMERIC_ATTRIBUTE_HPP
#define MI_DOUBLE_NUMERIC_ATTRIBUTE_HPP 1
#include "NumericAttribute.hpp"
namespace mi
{
        class AttributeSet;
        /**
         * @class DoubleNumericAttribute  DoubleNumericAttribute.hpp <mi/DoubleNumericAttribute.hpp>
         * @brief Attribute for DoubleNumeric object.
         */
        template <typename T>
        class DoubleNumericAttribute : public Attribute
        {
        public:
                explicit DoubleNumericAttribute ( const std::string& key,  NumericAttribute<T>* attr0,  NumericAttribute<T>* attr1 ) : Attribute( key ) {
                        this->_attr0 = attr0;
                        this->_attr1 = attr1;
                        return;
                }

                ~DoubleNumericAttribute( void ) {
                        delete this->_attr0;
                        delete this->_attr1;
                        return;
                }
                /**
                 * @brief Analyze all of childattributes.
                 * @retval true Success.
                 * @retval false Failure. Call AttributeSet::printError() to check which attribute is failed.
                 */
                bool parse ( const Argument& arg ) {
                        bool eval0 = this->_attr0->parse( arg );
                        bool eval1 = this->_attr1->parse( arg );
                        return eval0 && eval1;
                }
                /**
                 * @brief Print error messages on console.
                 */
                void printError( void ) {
                        this->_attr0->printError();
                        this->_attr1->printError();
                        return;
                }

                /**
                 * @brief Set minimum value.
                 * @param [in] minValue Minimum value.
                 * @return Itself.
                 */
                DoubleNumericAttribute<T>&  setMin( const T min0, const T min1 ) {
                        this->_attr0->setMin( min0 );
                        this->_attr1->setMin( min1 );
                        return *this;
                }

                /**
                 * @brief Set maximum value.
                 * @param [in] maxValue Maximum value.
                 * @return Itself.
                 */
                DoubleNumericAttribute<T>& setMax( const T max0, const T max1 ) {
                        this->_attr0->setMax( max0 );
                        this->_attr1->setMax( max1 );
                        return *this;
                }

                /**
                 * @brief Set minimum / maximum value.
                 * @param [in] minValue Minimum value.
                 * @param [in] maxValue Maximum value.
                 * @return Itself.
                 */
                DoubleNumericAttribute<T>& setMinMax(  const T min0, const T min1, const T max0, const T max1 ) {
                        return this->setMin ( min0, min1 ).setMax ( max0, max1 );
                }

                /**
                 * @brief Set default value.
                 * @param [in] defaultValue Default value.
                 * @return Itself.
                 */
                DoubleNumericAttribute<T>&  setDefaultValue ( const T default0, const T default1 ) {
                        this->_attr0->setDefaultValue( default0 );
                        this->_attr1->setDefaultValue( default1 );
                        return *this;
                }

                /**
                 * @brief Set this attribute must be within a range.
                 * @return Itself.
                 */
                DoubleNumericAttribute<T>& setOutRangeRejected( void ) {
                        this->_attr0->setOutRangeRejected();
                        this->_attr1->setOutRangeRejected();
                        return *this;
                }

                /**
                 * @brief Get value.
                 * @return Value.
                 */
                std::string toString( void ) const {
                        std::stringstream ss;
                        ss<<this->getKey()<<" : "<<this->getValue();
                        return ss.str();
                }

                std::string getValue( void ) const {
                        std::stringstream ss;
                        ss<<this->_attr0->getValue()<<" ";
                        ss<<this->_attr1->getValue();
                        return ss.str();
                }

                /**
                 * @brief Set this attribute is mandatory.
                 * @return Itself.
                 */
                DoubleNumericAttribute<T>& setMandatory( void ) {
                        this->_attr0->setMandatory();
                        this->_attr1->setMandatory();
                        return *this;
                }
                DoubleNumericAttribute<T>& setHidden ( void ) {
                        Attribute::setHidden();
                        return *this;
                }

                DoubleNumericAttribute<T>& setMessage( const std::string& message ) {
                        Attribute::setMessage( message ) ;
                        return *this;
                }
        private:
                NumericAttribute<T>* _attr0;
                NumericAttribute<T>* _attr1;
        };
}
#endif// MI_DOUBLE_NUMERIC_ATTRIBUTE_HPP
