/**
 * @file TripleNumericAttribute.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_TRIPLE_NUMERIC_ATTRIBUTE_HPP
#define MI_TRIPLE_NUMERIC_ATTRIBUTE_HPP 1
#include "NumericAttribute.hpp"
namespace mi
{
        class AttributeSet;
        /**
         * @class TripleNumericAttribute  TripleNumericAttribute.hpp <mi/TripleNumericAttribute.hpp>
         * @brief Attribute for TripleNumeric object.
         */
        template <typename T>
        class TripleNumericAttribute : public Attribute
        {
        public:
                explicit TripleNumericAttribute ( const std::string& key,  NumericAttribute<T>* attr0,  NumericAttribute<T>* attr1,  NumericAttribute<T>* attr2 ) : Attribute( key ) {
                        this->_attr0 = attr0;
                        this->_attr1 = attr1;
                        this->_attr2 = attr2;
                        return;
                }

                ~TripleNumericAttribute( void ) {
                        delete this->_attr0;
                        delete this->_attr1;
                        delete this->_attr2;
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
                        bool eval2 = this->_attr2->parse( arg );
                        return eval0 && eval1 && eval2;
                }
                /**
                 * @brief Print error messages on console.
                 */
                void printError( void ) {
                        this->_attr0->printError();
                        this->_attr1->printError();
                        this->_attr2->printError();
                        return;
                }
                /**
                 * @brief Print analysis result on console.
                 */
                /*                void print( void )
                                {
                                        std::cerr<<this->toString()<<std::endl;;
                                        return;
                                }
                */
                /**
                 * @brief Set minimum value.
                 * @param [in] minValue Minimum value.
                 * @return Itself.
                 */
                TripleNumericAttribute<T>&  setMin( const T min0, const T min1, const T min2 ) {
                        this->_attr0->setMin( min0 );
                        this->_attr1->setMin( min1 );
                        this->_attr2->setMin( min2 );
                        return *this;
                }

                /**
                 * @brief Set maximum value.
                 * @param [in] maxValue Maximum value.
                 * @return Itself.
                 */
                TripleNumericAttribute<T>& setMax( const T max0, const T max1, const T max2 ) {
                        this->_attr0->setMax( max0 );
                        this->_attr1->setMax( max1 );
                        this->_attr2->setMax( max2 );
                        return *this;
                }

                /**
                 * @brief Set minimum / maximum value.
                 * @param [in] minValue Minimum value.
                 * @param [in] maxValue Maximum value.
                 * @return Itself.
                 */
                TripleNumericAttribute<T>& setMinMax(  const T min0, const T min1, const T min2, const T max0, const T max1, const T max2 ) {
                        return this->setMin ( min0, min1, min2 ).setMax ( max0, max1, max2 );
                }

                /**
                 * @brief Set default value.
                 * @param [in] defaultValue Default value.
                 * @return Itself.
                 */
                TripleNumericAttribute<T>&  setDefaultValue ( const T default0, const T default1, const T default2 ) {
                        this->_attr0->setDefaultValue( default0 );
                        this->_attr1->setDefaultValue( default1 );
                        this->_attr2->setDefaultValue( default2 );
                        return *this;
                }

                /**
                 * @brief Set this attribute must be within a range.
                 * @return Itself.
                 */
                TripleNumericAttribute<T>& setOutRangeRejected( void ) {
                        this->_attr0->setOutRangeRejected();
                        this->_attr1->setOutRangeRejected();
                        this->_attr2->setOutRangeRejected();
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
                        ss<<this->_attr1->getValue()<<" ";
                        ss<<this->_attr2->getValue();
                        return ss.str();
                }

                /**
                 * @brief Set this attribute is mandatory.
                 * @return Itself.
                 */
                TripleNumericAttribute<T>& setMandatory( void ) {
                        this->_attr0->setMandatory();
                        this->_attr1->setMandatory();
                        this->_attr2->setMandatory();
                        return *this;
                }
                TripleNumericAttribute<T>& setHidden ( void ) {
                        Attribute::setHidden();
                        return *this;
                }

                TripleNumericAttribute<T>& setMessage( const std::string& message ) {
                        Attribute::setMessage( message ) ;
                        return *this;
                }
        private:
                NumericAttribute<T>* _attr0;
                NumericAttribute<T>* _attr1;
                NumericAttribute<T>* _attr2;
        };
}
#endif// MI_TRIPLE_NUMERIC_ATTRIBUTE_HPP
