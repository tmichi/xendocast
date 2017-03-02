/**
* @file Kdtree.hpp
* @author Takashi Michikawa <michikawa@acm.org>
*/
#pragma once
#ifndef MI_KDTREE_HPP
#define MI_KDTREE_HPP 1

#include <cmath>
#include <cstdlib>
#include <vector>
#include <list>
#include <algorithm>

namespace mi
{
        /**
        * @class Kdtree Kdtree.hpp "mi/Kdtree.hpp"
        * @note You can integrate with any own vector types, but it must have  following methods:
        * @li T::operator[](int); to access each coordinate value.
        * @li T::T(const T& d);
        * @li T::copy( const T& d) copying entity of d.
        */
        template <typename T, size_t Dim = 3>
        class Kdtree
        {
        private:
                /**
                * @class node in kdtree
                */

                class Node
                {
                private:
                        class less_vec_coord
                        {
                        private:
                                char _dim;
                        public:
                                less_vec_coord ( const char dim ) : _dim ( dim ) {
                                        return;
                                }
                                bool operator() ( const T &a, const T &b ) const {
                                        return a[_dim] < b[_dim];
                                }
                        };
                private:
                        char		_dimension; // X (0), Y (1) or Z (2)
                        Node* 	        _child;
                        std::list<T> 	_points;
                public:
                        Node ( void ) : _dimension( -1 ), _child( NULL ) {
                                return;
                        }

                        ~Node ( void ) {
                                this->remove_children();
                                return;
                        }

                        bool init ( typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end, const size_t numMaxNode ) {
                                this->remove_children();
                                this->_points.clear();

                                const size_t numElements = std::distance ( begin, end );
                                if ( numElements <= numMaxNode ) {
                                        this->_points.insert( this->_points.end(), begin, end );
                                } else {
                                        this->_dimension = this->find_separation_axis( begin, end );
                                        std::sort ( begin, end, less_vec_coord ( this->_dimension ) );
                                        typename std::vector<T>::iterator center = begin + static_cast<size_t> ( numElements / 2 );
                                        this->_points.push_back ( *center );

                                        this->create_children();
                                        this->_child[0].init ( begin , center, numMaxNode );
                                        this->_child[1].init ( center, end   , numMaxNode );
                                }
                                return true;
                        }

                        bool isLeaf ( void ) const {
                                return this->_dimension == -1;
                        }

                        void find ( const T &pnt, const double radius, typename std::list<T>& result ) {
                                if ( this->_points.empty() ) return;
                                if ( this->isLeaf() ) {
                                        typename std::list<T>::iterator iter;
                                        const double sqr = radius * radius;
                                        for ( iter = this->_points.begin() ; iter != this->_points.end() ; iter++ ) {
                                                double check = 0;
                                                for ( size_t i = 0 ; i < Dim ; i++ ) check += ( iter->operator[] ( i ) - pnt[i] ) * ( iter->operator[] ( i ) - pnt[i] );
                                                if  ( check <= sqr )  result.push_back ( *iter );
                                        }
                                } else {
                                        const double p = this->_points.begin()->operator[] ( this->_dimension ); // separator
                                        const double x = pnt[this->_dimension]; // target
                                        if ( fabs ( x-p ) <= radius ) {
                                                this->_child[0].find ( pnt, radius, result );
                                                this->_child[1].find ( pnt, radius, result );
                                        } else {
                                                const int idx = ( x < p ) ? 0 : 1;
                                                this->_child[idx].find ( pnt, radius, result );
                                        }
                                }
                                return;
                        }

                        void add ( const T& p, const size_t numMaxElementsPerNode ) {
                                if ( this->isLeaf() ) {
                                        this->_points.push_back ( p );
                                        if ( this->_points.size() > numMaxElementsPerNode ) {
                                                std::vector<T> points ( this->_points.begin(), this->_points.end() );
                                                this->_points.clear();
                                                this->init ( points.begin(), points.end(), numMaxElementsPerNode );
                                                points.clear();
                                        }
                                } else {
                                        const double delim = this->_points.begin()->operator[] ( this->_dimension );
                                        const double x = p[this->_dimension];
                                        const int idx = ( x < delim ) ? 0 : 1;
                                        this->_child[idx].add ( p, numMaxElementsPerNode );
                                };
                                return;
                        };


                        void getAll ( std::vector<T>& point ) {
                                if ( this->isLeaf() ) {
                                        point.insert ( point.end(), this->_points.begin(), this->_points.end() );
                                } else {
                                        this->_child[0].getAll ( point );
                                        this->_child[1].getAll ( point );
                                }
                                return;
                        }
                private:
                        void remove_children ( void ) {
                                if ( this->_child != NULL ) {
                                        delete[] this->_child;
                                        this->_child = NULL;
                                }
                                return;
                        }


                        void create_children( void ) {
                                this->_child = new Node[2];
                                return;
                        }

                        char find_separation_axis ( typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end ) const {
                                T bmin = *begin;
                                T bmax = *begin;
                                for ( typename std::vector<T>::iterator iter = begin ; iter != end ; ++iter ) {
                                        T p = *iter;
                                        for( size_t i = 0 ; i < Dim ; i++ ) {
                                                if ( p[i] < bmin[i] ) bmin[i] = p[i];
                                                if ( p[i] > bmax[i] ) bmax[i] = p[i];
                                        }
                                }
                                char maxDim = 0;
                                double maxDeviation = bmax[0] - bmin[0];
                                for( size_t i = 0 ; i < Dim ; i++ ) {
                                        const double deviation = bmax[i] - bmin[i];
                                        if( maxDeviation < deviation ) {
                                                maxDim = static_cast<char>( i );
                                                maxDeviation = deviation;
                                        }
                                }
                                return maxDim;
                        }
                };
        private:
                Kdtree( const Kdtree& that );
                void operator = ( const Kdtree& that );
        private:
                size_t  _numElement;
                size_t  _numMaxElementsPerNode;
                Node _parent;

                double _init_radius;
        public:
                explicit Kdtree ( void ) : _numElement( 0 ), _numMaxElementsPerNode( 10 ), _init_radius( 0.001 ) {
                        return;
                }

                explicit Kdtree ( std::vector<T>& point, const size_t numMaxElementsPerNode = 10 , const double init_radius = 0.001 ) {
                        this->build ( point, numMaxElementsPerNode , init_radius );
                        return;
                }

                ~Kdtree ( void ) {
                        return;
                }

                bool build ( std::vector<T>& point, const size_t numMaxElementsPerNode = 10 , const double init_radius = 0.001 ) {
                        this->_numMaxElementsPerNode = numMaxElementsPerNode;
                        this->_numElement = point.size();
                        this->_init_radius = init_radius;
                        return this->_parent.init ( point.begin(), point.end(), numMaxElementsPerNode );
                }

                bool rebuild ( const size_t numMaxElementsPerNode = 10 ) {
                        std::vector<T> point;
                        point.reserve ( this->size() );
                        this->_parent.getAll ( point );
                        return this->build ( point, numMaxElementsPerNode );
                }
                void find ( const T p, const double radius, std::vector<T>& node, bool isSorted = false ) {
                        node.clear();
                        std::list<T> result;
                        this->find( p, radius, result, isSorted );
                        node.insert( node.end(), result.begin(), result.end() );
                        return;
                }

                void find ( const T p, const double radius, std::list<T>& node, bool isSorted = false ) {
                        node.clear();
                        this->_parent.find ( p, radius, node );
                        if ( isSorted ) node.sort ( less_vec_length ( p ) );
                        return;
                }
                void find ( const T p, const size_t num, std::vector<T>& node ) {
                        node.clear();
                        std::list<T> result;
                        this->find( p, num, result );
                        node.insert( node.end(), result.begin(), result.end() );
                        return;

                }
                void find ( const T p, const size_t num, std::list<T>& node ) {
                        double r = this->_init_radius;
                        const double base = std::pow ( static_cast<double> ( num ), 1.0/3.0 );
                        node.clear();
                        if ( this->size () < num ) {
                                std::vector<T> all;
                                this->_parent.getAll( all );
                                node.insert( node.end(), all.begin(), all.end() );
                        } else {
                                while ( node.size() < num ) {
                                        this->find ( p, r, node, false );
                                        if ( node.size() == 0 ) r *= 2;
                                        else  r *= static_cast<double> ( base * 1.0 / pow ( static_cast<double>( node.size() ), 0.3333 ) );
                                }

                                node.sort ( less_vec_length ( p ) );
                                node.resize ( num );

                        }
                        return;
                }

                T closest ( const T& p ) {
                        std::list<T> node;
                        node.clear();
                        double radius = this->_init_radius;
                        while ( node.empty() ) {
                                radius *= 2;
                                this->find ( p, radius, node, true );
                        }
                        return * ( node.begin() );
                }

                void add ( const T& element ) {
                        this->_parent.add ( element, this->_numMaxElementsPerNode );
                        this->_numElement += 1;
                        return;
                }

                size_t size ( void ) const {
                        return this->_numElement;
                }
        private:
                /**
                * @class less_vec_length
                * @brief functor class for comparing two vector used in Kdtree.
                */
                class less_vec_length
                {
                private:
                        T _x;
                public:
                        less_vec_length ( const T x ) : _x ( x ) {
                                return;
                        }
                        bool operator () ( const T& a, const T& b ) const {
                                double ra = 0;
                                double rb = 0;
                                for ( size_t i  = 0 ; i  < Dim ; i++ ) {
                                        ra += ( a[i] - this->_x[i] ) * ( a[i] - this->_x[i] );
                                        rb += ( b[i] - this->_x[i] ) * ( b[i] - this->_x[i] );
                                }
                                return ra < rb ;
                        }
                };
        };


        template <typename T, size_t Dim = 3>
        class IndexedVector : public T
        {
        private:
                int _id;
        public:

                IndexedVector ( const T &v = T(), const int id = -1 ) : T ( v ), _id( id ) {
                        return;
                }

                IndexedVector ( const IndexedVector<T>& that ) {
                        this->copy ( that );
                        return;
                }

                IndexedVector& operator = ( const IndexedVector<T>& that ) {
                        this->copy ( that );
                        return *this;
                }
                /**
                * @brief get id of the point.
                * @return ID of the point.
                */
                int id ( void ) const {
                        return this->_id;
                }
        protected:
                void copy ( const IndexedVector<T>& that ) {
                        for( int i = 0 ; i < Dim ; i++ ) {
                                this->operator[]( i ) = that.operator[]( i );
                        }
                        this->_id = that._id;
                        return;
                }
        };//IndexedVector
}
#endif// MI_KDTREE_HPP











