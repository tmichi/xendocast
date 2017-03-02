/**
 * @file MeshStitcher.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_MESH_STITCHER_HPP
#define MI_MESH_STITCHER_HPP 1

#include <vector>
#include "Mesh.hpp"
#include "Kdtree.hpp"
namespace mi
{
        /**
        * @class MeshStitcher MeshStitcher.hpp <mi/MeshStitcher.hpp>
        * @brief Stl Importer.
        */
        class  MeshStitcher
        {
        private:
                const Mesh& _mesh;
                const double _eps;
        public:
                /**
                 * @brief Constructor.
                 * @param [in] mesh Mesh Stlect.
                 */
                explicit MeshStitcher ( const Mesh& mesh , const double eps = 1.0e-10 ) : _mesh( mesh ), _eps( eps ) {
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                virtual ~MeshStitcher ( void ) {
                        return;
                }

                bool stitch ( Mesh& resultMesh ) {
                        typedef IndexedVector<Vector3d> VertexType;
                        std::vector<int> newId( this->_mesh.getNumVertices(), -1 ) ;
                        std::vector< VertexType > points;
                        for( int i = 0 ; i < this->_mesh.getNumVertices() ; ++i ) {
                                points.push_back( VertexType( this->_mesh.getPosition( i ), i ) ) ;
                        }
                        Kdtree<VertexType> kdtree( points );
                        for( int i = 0 ; i < this->_mesh.getNumVertices() ; ++i ) {
                                if ( newId[i] != -1 ) continue;
                                std::list<VertexType> result;
                                kdtree.find( VertexType( this->_mesh.getPosition( i ), 0 ), this->_eps, result );
                                const int id = resultMesh.addPoint( this->_mesh.getPosition( i ) ) ;
                                for( std::list<VertexType>::iterator iter = result.begin() ; iter != result.end() ; ++iter ) {
                                        newId[ iter->id() ] = id;
                                }
                        }

                        for ( int i = 0 ; i < this->_mesh.getNumFaces() ; ++i ) {
                                std::vector<int> index = this->_mesh.getFaceIndices ( i ) ;
                                for ( size_t j = 0 ; j < index.size() ; ++j ) {
                                        const int id = index[j];
                                        index[j] = newId [ id ];
                                }
                                resultMesh.addFace( index );
                        }
                        return true;
                }

        };
}
#endif// MI_MESH_STITCHER_HPP

