/**
 * @file MeshImporterObj.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_MESH_IMPORTER_OBJ_HPP
#define MI_MESH_IMPORTER_OBJ_HPP 1

#include <vector>
#include "Tokenizer.hpp"
#include "MeshImporter.hpp"

namespace mi
{
        /**
        * @class MeshImporterObj MeshImporterObj.hpp <mi/MeshImporterObj.hpp>
        * @brief Obj Importer.
        */
        class  MeshImporterObj : public MeshImporter
        {
        public:
                /**
                 * @brief Constructor.
                 * @param [in] mesh Mesh object.
                 */
                explicit MeshImporterObj ( Mesh& mesh ) :  MeshImporter( mesh, false ) {
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                virtual ~MeshImporterObj ( void ) {
                        return;
                }
        private:
                /**
                 * @brief Write body part of STL file.
                 * @param [in] File stream.
                 */
                virtual bool readBody ( std::ifstream &fin ) {
                        Mesh& mesh = this->getMesh();

                        std::vector<Eigen::Vector3d> vx;
                        std::deque< std::deque<int> > index;

                        int count = -1;
                        while ( fin ) {
                                std::string  buffer;
                                std::getline( fin, buffer );
                                if( fin.eof() ) break;
                                if ( buffer.find_first_of( "#" ) == 0 ) continue;
                                if ( buffer.length() == 0 ) continue;

                                Tokenizer tokenizer1 ( buffer, " " ) ;
                                if ( tokenizer1.size() == 0 ) continue;
                                if ( tokenizer1.get( 0 ).compare( "v" ) == 0 ) {
                                        Eigen::Vector3d p;
                                        p.x() = std::atof( tokenizer1.get( 1 ).c_str() );
                                        p.y() = std::atof( tokenizer1.get( 2 ).c_str() );
                                        p.z() = std::atof( tokenizer1.get( 3 ).c_str() );
                                        vx.push_back( p );
                                } else if ( tokenizer1.get( 0 ).compare( "g" ) == 0 ) {
                                        count++;
                                } else if ( tokenizer1.get( 0 ).compare( "f" ) == 0 ) {
                                        index.push_back( std::deque<int>() );
                                        for( int i = 1 ; i < tokenizer1.size() ; ++i ) {
                                                Tokenizer tokenizer2( tokenizer1.get( i ), "/" );
                                                if ( tokenizer2.get( 0 ).length() != 0 ) {
                                                        std::string token = tokenizer2.get( 0 );
                                                        int fid =  std::atoi( token.c_str() );
                                                        if( fid != 0 ) index[ index.size() - 1 ].push_back( fid - 1 );

                                                }
                                        }
                                }
                        }

                        mesh.init();
                        for ( size_t i = 0 ; i < vx.size() ; ++i ) {
                                mesh.addPoint( vx[i] );
                        }
                        for( size_t j = 0 ; j < index.size() ; ++j ) {
                                //N-gon -> triangles
                                for ( size_t k = 0  ; k < index[j].size() - 2; ++k ) {
                                        std::vector<int> fid;
                                        fid.push_back( index[j][0] );
                                        fid.push_back( index[j][k+1] );
                                        fid.push_back( index[j][k+2] );
                                        mesh.addFace( fid );
                                }
                        }

                        return true;
                }
        public:
                /**
                 * @brief Return type of expoter.
                 * @return String value.
                 */
                virtual std::string toString ( void ) const {
                        return std::string ( "obj" );
                }
        };
}
#endif// MI_MESH_ImportER_OBJ_HPP

