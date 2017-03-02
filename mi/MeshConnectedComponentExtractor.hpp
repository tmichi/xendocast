#ifndef MI_MESH_CONNECTED_COMPONENT_EXTRACTOR_HPP
#define MI_MESH_CONNECTED_COMPONENT_EXTRACTOR_HPP 1
#include "Mesh.hpp"
#include "AssemblyMesh.hpp"
#include "Graph.hpp"
#include <deque>
#include <utility>
#include <algorithm>
#include <stack>
namespace mi
{
        class MeshConnectedComponentExtractor
        {
        private:
                const Mesh& _mesh;
        public:
                MeshConnectedComponentExtractor ( const Mesh& mesh ) : _mesh ( mesh ) {
                        return;
                }

                ~MeshConnectedComponentExtractor ( void ) {
                        return;
                }

                int extract ( AssemblyMesh& assy ) {
                        const Mesh& mesh = this->_mesh;
                        std::vector<int> faceLabel ( mesh.getNumFaces(), -1 ) ;

                        std::deque< std::pair<int, int> > vfpairs; // vid , fid
                        for ( int i = 0 ; i < mesh.getNumFaces() ; ++i ) {
                                std::vector<int> index = mesh.getFaceIndices ( i );
                                const int numvf = static_cast<int>( index.size() );
                                for( int j = 0 ; j < numvf ; ++j ) {
                                        vfpairs.push_back ( std::make_pair( index[j], i ) ) ;
                                }
                        }
                        std::sort ( vfpairs.begin(), vfpairs.end() );

                        std::vector<int> vidx;
                        int curvid = -1;
                        for( int i = 0 ; i < vfpairs.size() ; ++i ) {
                                while ( vfpairs[i].first  != curvid ) {
                                        vidx.push_back( i );
                                        ++curvid;
                                }
                        }
                        vidx.push_back( static_cast<int>( vfpairs.size() ) );


                        std::deque<std::pair<int, int> > edges;
                        std::vector<int> idlist;

                        for ( int i = 0 ; i < vidx.size() ; ++i ) {
                                std::vector<int> fids;
                                const int begin = vidx[i];
                                const int end   = vidx[i+1];
                                for( int j = begin ; j < end ; ++j ) {
                                        fids.push_back( vfpairs[j].second );
                                }

                                std::sort( fids.begin(), fids.end() );
                                for( int j = 0 ; j < fids.size() ; ++j ) {
                                        for( int k = j + 1 ; k < fids.size() ; ++k ) {
                                                edges.push_back (  std::make_pair( fids[j], fids[k]  ) ) ;
                                        }
                                }
                        }

                        vfpairs.clear();
                        std::sort ( edges.begin(), edges.end() );
                        std::deque<std::pair<int,int> >::iterator eiter = std::unique ( edges.begin(), edges.end() );
                        edges.erase( eiter, edges.end() );

                        Graph graph ( mesh.getNumFaces() );
                        for ( int i = 0 ; i < edges.size() ; ++i ) {
                                graph.addEdge ( edges[i].first, edges[i].second );
                        }
                        const int numLabels = graph.label();

                        for ( int i = 0 ; i < numLabels ; ++i ) {
                                assy.create();
                        }

                        for ( int i = 0 ; i < mesh.getNumFaces() ; ++i ) {
                                const int id = graph.getClusterId( i );
                                std::vector<int> index = mesh.getFaceIndices ( i );


                                for( int j = 0 ; j < index.size() ; ++j ) {
                                        const mi::Vector3d p = mesh.getPosition( index[j] );
                                        int newId = assy.getMesh( id )->addPoint( p );
                                        index[j] = newId;
                                }
                                assy.getMesh( id )->addFace( index );
                        }

                        return numLabels;
                }

        };
}
#endif //MI_MESH_CONNECTED_COMPONENT_EXTRACTOR_HPP
