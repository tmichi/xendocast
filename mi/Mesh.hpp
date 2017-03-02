/**
 * @file Mesh.hpp
 * @brief
 */
#ifndef MI_MESH_HPP
#define MI_MESH_HPP 1
#include <iostream>
#include <vector>
#include <utility>
#include <deque>
#include <map>
#include <cmath>
#include "math.hpp"

namespace mi
{
        class Mesh
        {
        private:
                Mesh( const Mesh& that );
                void operator = ( const Mesh& that );
        private:
                std::string _name;
                std::vector<mi::Vector3d> _vertex; /// vertex position
                std::vector<int> _index; /// triangles
        public:
                explicit Mesh ( void ) {
                        this->init();
                        return;
                }

                virtual ~Mesh ( void ) {
                        return;
                }

                void clone ( const Mesh &mesh ) {
                        this->init();
                        for( int i = 0 ; i < mesh.getNumVertices() ; ++i ) {
                                this->addPoint( mesh.getPosition( i ) );
                        }
                        for( int i = 0 ; i < mesh.getNumFaces() ; ++i ) {
                                std::vector<int> index = mesh.getFaceIndices( i );
                                this->addFace( index );
                        }
                }

                int addPoint( const mi::Vector3d& p ) {
                        this->_vertex.push_back( p );
                        return static_cast<int>( this->_vertex.size() - 1 );
                }

                int addFace ( const std::vector<int>& fidx ) {
                        if ( fidx.size() != 3 ) return -1;
                        this->_index.insert ( this->_index.end(), fidx.begin(), fidx.end() );
                        return static_cast<int>( this->_index.size() / 3 ) ; // ID
                }

                void addName ( const std::string name = std::string( "mesh" ) ) {
                        this->_name = name;
                        return;
                }

                inline bool isValidFaceId( const int faceid ) const {
                        return ( 0 <= faceid && faceid < this->getNumFaces() );
                }

                inline bool isValidVertexId( const int vertexid ) const {
                        return ( 0 <= vertexid && vertexid < this->getNumVertices() );
                }

                inline std::vector<int> getFaceIndices ( const int faceid ) const {
                        std::vector<int> idx ( 3, -1 );
                        if ( this->isValidFaceId( faceid ) ) {
                                for ( int i = 0 ; i < 3 ; i++ ) {
                                        idx[i] = this->_index.at ( faceid * 3 + i );
                                }
                        }
                        return idx;
                }

                inline mi::Vector3d getPosition ( const int vertexid ) const {
                        if ( this->isValidVertexId( vertexid ) ) return this->_vertex.at ( vertexid );
                        return mi::Vector3d();
                }


                mi::Vector3d getNormal ( const int faceid , bool normalize = true ) const {
                        if ( ! this->isValidFaceId( faceid ) ) return mi::Vector3d();

                        std::vector<int> fidx = this->getFaceIndices( faceid );
                        mi::Vector3d v0 = this->getPosition( fidx[0] );
                        mi::Vector3d v1 = this->getPosition( fidx[1] ) - v0;
                        mi::Vector3d v2 = this->getPosition( fidx[2] ) - v0;
                        mi::Vector3d n = v1.cross( v2 );
                        if ( normalize ) n.normalize();
                        return n;
                }

                inline std::string
                getName ( void ) const {
                        return this->_name;
                }


                void setPosition ( const int vertexid, const mi::Vector3d& pos ) {
                        if ( this->isValidVertexId( vertexid ) ) {
                                this->_vertex.at ( vertexid ) = pos;
                        }
                        return;
                };
                void init( void ) {
                        this->_vertex.clear();
                        this->_index.clear();
                        return;
                }

                inline int getNumVertices( void ) const {
                        return static_cast<int>( this->_vertex.size() );
                }

                inline int getNumFaces( void ) const {
                        return static_cast<int>( this->_index.size() / 3 );
                }

                void
                getBoundingBox ( mi::Vector3d& bmin, mi::Vector3d& bmax ) {
                        const size_t numv = this->getNumVertices();
                        if ( numv == 0 ) {
                                std::cerr<<"No vertices. return nothing."<<std::endl;
                                return;
                        }
                        bmin = this->getPosition ( 0 );
                        bmax = this->getPosition ( 0 );
                        for ( size_t i = 1 ; i < numv ; ++i ) {
                                const mi::Vector3d v = this->getPosition ( static_cast<int>( i ) );
                                if ( v.x() < bmin.x() ) bmin.x() = v.x();
                                if ( v.y() < bmin.y() ) bmin.y() = v.y();
                                if ( v.z() < bmin.z() ) bmin.z() = v.z();

                                if ( v.x() > bmax.x() ) bmax.x() = v.x();
                                if ( v.y() > bmax.y() ) bmax.y() = v.y();
                                if ( v.z() > bmax.z() ) bmax.z() = v.z();
                        }
                        return ;
                }

                void negateOrientation() {
                        for( int i = 0 ; i < this->getNumFaces() ; ++i ) {
                                int f0 = this->_index [ i * 3 + 0 ];
                                this->_index[ i * 3 + 0 ] = this->_index [ i * 3 + 1];
                                this->_index[ i * 3 + 1 ] = f0;

                        }

                }

                double getArea( const int faceId ) const {
                        std::vector<int> index = this->getFaceIndices( faceId );
                        const Eigen::Vector3d p0 = this->getPosition( index[0] );
                        const Eigen::Vector3d p1 = this->getPosition( index[1] );
                        const Eigen::Vector3d p2 = this->getPosition( index[2] );
                        const Eigen::Vector3d v0 = p1 - p0;
                        const Eigen::Vector3d v1 = p2 - p0;
                        return v0.cross( v1 ).norm() * 0.5; //|v0^v1|/2
                }
        };
}//namespace mi
#endif
