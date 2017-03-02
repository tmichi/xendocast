/**
 * @file AttributeSet.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_ATTRIBUTE_SET_HPP
#define MI_ATTRIBUTE_SET_HPP 1

#include <list>
#include <string>

#include "Attribute.hpp"
#include "BooleanAttribute.hpp"
#include "NumericAttribute.hpp"
#include "StringAttribute.hpp"
#include "DoubleNumericAttribute.hpp"
#include "TripleNumericAttribute.hpp"
namespace mi
{
        /**
         * @class AttributeSet "AttributeSet.hpp"  <mi/AttributeSet.hpp>
         * @brief Attribute set.
         *
         * This class represents a set of attributes.
         */
        class AttributeSet : public Attribute
        {
        protected:
                typedef std::list<Attribute*>::iterator Iterator;
        public:
                /**
                 * @brief Constructor.
                 */
                explicit AttributeSet ( void ) : Attribute( "" ) , _isAnd( true ) {
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                virtual ~AttributeSet ( void ) {
                        for( Iterator iter = this->_attr.begin() ; iter != this->_attr.end() ; ++iter ) {
                                delete ( *iter );
                        }
                        return ;
                }

                /**
                 * @brief Set this class is "OR".
                 *
                 * When this method is called, return ATTRIBUTE_ERROR_OK if one of attributes is analyzed.
                 * @return Object itself.
                 */
                AttributeSet& setOr ( void ) {
                        this->_isAnd = false;
                        return *this;
                }

                /**
                 * @brief Return instance of AttributeSet.
                 * @return The isntance.
                 * @note Created instance is released at AttributeSet::~AttributeSet(), or you do not have to take care it.
                 */
                AttributeSet& createAttributeSet( void ) {
                        Attribute* sAttr = new AttributeSet();
                        this->_attr.push_back( sAttr );
                        return dynamic_cast<AttributeSet&>( *sAttr );
                }

                /**
                 * @brief Return instance of BooleanAttribute.
                 * @return The isntance.
                 * @note Created instance is released at AttributeSet::~AttributeSet(), or you do not have to take care it.
                 */
                BooleanAttribute& createBooleanAttribute( const std::string& key, bool& value , const std::string& message = std::string() ) {
                        Attribute* bAttr = new BooleanAttribute ( key, value ) ;
                        bAttr->setMessage( message );
                        this->_attr.push_back( bAttr );
                        return dynamic_cast<BooleanAttribute& >( *bAttr );
                }

                /**
                 * @brief Return instance of StringAttribute.
                 * @return he isntance.
                 * @note Created instance is released at AttributeSet::~AttributeSet(), or you do not have to take care it.
                 */
                StringAttribute& createStringAttribute( const std::string& key, std::string& value , const std::string &message = std::string() ) {
                        Attribute* sAttr = new StringAttribute ( key, value ) ;
                        sAttr->setMessage( message );
                        this->_attr.push_back( sAttr );
                        return dynamic_cast<StringAttribute&>( *sAttr );
                }

                /**
                 * @brief Return instance of NumericAttribute.
                 * @return The isntance.
                 * @note Created instance is released at AttributeSet::~AttributeSet(), or you do not have to take care it.
                 */
                template<typename T>
                NumericAttribute<T>& createNumericAttribute( const std::string& key, T& value,  const std::string &message = std::string() ) {
                        Attribute* nAttr = new NumericAttribute<T> ( key, value ,1 ) ;
                        nAttr->setMessage( message );
                        this->_attr.push_back( nAttr );
                        return dynamic_cast<NumericAttribute<T>& >( *nAttr );
                }

                template<typename T>
                DoubleNumericAttribute<T>& createDoubleNumericAttribute( const std::string& key, T& value0, T& value1, const std::string &message = std::string() ) {
                        /// @todo should allocate memory in Double Numeric Attribute.
                        NumericAttribute<T>* nAttr0 = new NumericAttribute<T> ( key, value0 , 1 );
                        NumericAttribute<T>* nAttr1 = new NumericAttribute<T> ( key, value1 , 2 );
                        Attribute* dAttr = new DoubleNumericAttribute<T>( key, nAttr0, nAttr1 );
                        dAttr->setMessage( message );
                        this->_attr.push_back( dAttr );
                        return dynamic_cast<DoubleNumericAttribute<T>& >( *dAttr );
                }

                template<typename T>
                TripleNumericAttribute<T>& createTripleNumericAttribute( const std::string& key, T& value0, T& value1, T& value2, const std::string &message = std::string() ) {
                        NumericAttribute<T>* nAttr0 = new NumericAttribute<T> ( key, value0 , 1 );
                        NumericAttribute<T>* nAttr1 = new NumericAttribute<T> ( key, value1 , 2 );
                        NumericAttribute<T>* nAttr2 = new NumericAttribute<T> ( key, value2 , 3 );
                        Attribute* tAttr = new TripleNumericAttribute<T>( key, nAttr0, nAttr1, nAttr2 );
                        tAttr->setMessage( message );
                        this->_attr.push_back( tAttr );
                        return dynamic_cast<TripleNumericAttribute<T>& >( *tAttr );
                }


                /**
                 * @brief Analyze all of childattributes.
                 * @retval true Success.
                 * @retval false Failure. Call AttributeSet::printError() to check which attribute is failed.
                 */
                bool analyze ( const Argument& arg ) {
                        return this->parse( arg );
                }

                bool parse( const Argument& arg ) {
                        bool result;
                        if( this->_isAnd ) {
                                result = true;
                                for( Iterator iter = this->_attr.begin() ; iter != this->_attr.end() ; ++iter ) {
                                        result &= ( *iter )->parse( arg );
                                }
                        } else {
                                result  = false;
                                for( Iterator iter = this->_attr.begin() ; iter != this->_attr.end() ; ++iter ) {
                                        result |=  ( *iter )->parse( arg );
                                }
                        }
                        if ( !result ) {
                                std::cerr<<"error"<<std::endl;
                                this->printError();
                        }
                        return result;

                }

                /**
                 * @brief Print error messages on console.
                 */
                void printError( void ) {
                        for( Iterator iter = this->_attr.begin() ; iter != this->_attr.end() ; iter++ ) {
                                ( *iter )->printError();
                        }
                        return;
                }
                /**
                 * @brief Print analysis result on console.
                 */
                void print( std::ostream& out = std::cout ) {
                        out<<"Arguments: "<<std::endl;
                        for( Iterator iter = this->_attr.begin() ; iter != this->_attr.end() ; iter++ ) {
                                ( *iter )->print( out );
                        }
                        return;
                }


                /**
                 * @brief print string.
                 * @return "Attribute set".
                 */
                std::string toString ( void ) const {
                        return std::string ( "Attribute set" );
                }


                void printUsage ( const std::string cmdStr ) {
                        std::cerr<<"Usage : "<<cmdStr<<" [OPTIONS]"<<std::endl
                                 <<std::endl
                                 <<"OPTIONS:"<<std::endl;
                        this->print_usage();
                }


                virtual void print_usage ( void ) {
                        for( Iterator iter = this->_attr.begin() ; iter != this->_attr.end() ; iter++ ) {
                                ( *iter )->print_usage();
                        }
                }
        private:
                bool _isAnd; ///< Flag whether this attribute set is "AND".
        protected:
                std::list<Attribute*> _attr; ///< List of attributes.
        };
};
#endif //MI_ATTRIBUTE_SET_HPP
