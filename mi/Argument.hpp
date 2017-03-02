/**
 * @file Argument.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_ARGUMENT_HPP
#define MI_ARGUMENT_HPP 1

#include <deque>
#include <string>
#include <iostream>

#include "Parse.hpp"

namespace mi
{
        /**
         * @class Argument Argument.hpp <mi/Argument.hpp>
         * @brief Argument object.
         * @sa AttributeSet, NumericAttribute, BooleanAttribute, StringAttribute
         */
        class Argument
        {
        private:
                Argument ( const Argument& that );
                void operator = ( const Argument& that );
        public:
                /**
                 * @brief Constructor.
                 * @param [in] argc The number of argv strings. This corresponds to argc in main().
                 * @param [in] argv Argument strings. This corresponds to argv in main().
                 */
                explicit Argument ( int argc = 0 , char** argv = NULL ) {
                        for ( int i = 0 ; i < argc ; ++i ) {
                                this->add( std::string ( argv[i] ) );
                        }
                        return;
                }

                /**
                 * @brief Destructor.
                 */
                ~Argument ( void ) {
                        return;
                }

                /**
                 * @brief Get size of argument array.
                 * @return The number of arguments.
                 */
                inline int size ( void ) const {
                        return static_cast<int>( this->_argv.size() );
                }

                /**
                 * @brief Add argument string.
                 * @param [in] str Arguments ( not parsed.)
                 * @return Argument object.
                 */
                Argument& add ( const std::string& str ) {
                        this->_argv.push_back ( str );
                        return *this;
                }
                /**
                 * @brief Check the key exists or not.
                 * @param [in] key Key.
                 * @param [in] offset Offset value.
                 * @retval true The key exists.
                 * @retval false The key does not exist.
                 */
                inline bool exist ( const std::string& key, const int offset = 0 ) const {
                        return ( this->find( key, offset ) > -1 );
                }

                /**
                 * @brief Get value associated with the key.
                 * @param [in] key Key.
                 * @param [in] offset Offset value.
                 * @return Value associated with the key. return 0 if the key does not exist.
                 */
                template <typename T>
                inline T get ( const std::string& key, const int offset = 1 ) const {
                        const int index = this->find ( key , offset );
                        return this->get<T>( index ) ;
                };

                /**
                 * @brief get value at i-th argument.
                 * @param [in] idx Index.
                 * @return i-th argument. return 0 when the index is invalid.
                 */
                template <typename T>
                inline T get ( const int idx ) const {
                        if ( !this->is_valid ( idx ) ) return T();
                        else return mi::parse<T> ( this->_argv.at ( idx ) ) ;
                }

                /**
                 *  @brief Find index of the key.
                 *  @param [in] key Key.
                 *  @param [in] offset Offset value.
                 *  @return Index of the value. Return -1 when offset value is invalid or the key does not exist.
                 */
                inline int find ( const std::string& key, const int offset = 0 ) const {
                        if ( offset < 0 ) return -1;
                        for ( int i = 0 ; i < this->size() - offset ; ++i ) {
                                if ( this->get<std::string>( i ).compare( key ) == 0 ) return i + offset;
                        }
                        return -1; // any arguments are not matched.
                };

                /**
                 * @brief Print arguments.
                 * @param [in] out
                 */
                void print ( std::ostream& out ) {
                        for( int i = 0  ; i < this->size() ; ++i ) {
                                std::string str = this->get<std::string>( i );
                                if ( str.find( "-", 0 ) == 0 ) out<<std::endl; //
                                out << str<<" ";
                        }
                        out<<std::endl;
                }
        private:
                inline bool is_valid ( const int idx ) const {
                        return ( 0 <= idx && idx < this->size() );
                }
        private:
                std::deque<std::string>  _argv; ///< Arguments.
        };
};
#endif //MI_ARGUMENT_HPP
