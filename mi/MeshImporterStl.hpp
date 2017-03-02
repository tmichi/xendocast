/**
 * @file MeshImporterStl.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_MESH_IMPORTER_STL_HPP
#define MI_MESH_IMPORTER_STL_HPP 1

#include <vector>
#include "MeshImporter.hpp"

namespace mi
{
        /**
        * @class MeshImporterStl MeshImporterStl.hpp <mi/MeshImporterStl.hpp>
        * @brief Stl Importer.
        */
        class  MeshImporterStl : public MeshImporter
        {
        private:
                std::string _name;
                unsigned int _num_faces;
        public:
                /**
                 * @brief Constructor.
                 * @param [in] mesh Mesh Stlect.
                 */
                explicit MeshImporterStl ( Mesh& mesh , bool isBinary = true ) :  MeshImporter( mesh, isBinary ) {
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                virtual ~MeshImporterStl ( void ) {
                        return;
                }
        private:
                bool readHeader ( std::ifstream& fin ) {
                        if ( !this->isBinary() ) return true;
                        //Binary
                        char buf[80];
                        fin.read ( ( char* ) &buf, 80 );
                        this->_name = std::string ( buf );
                        fin.read( ( char* )&( this->_num_faces ), sizeof ( unsigned int ) );

                        return true; fin.good();
                }
                /**
                 * @brief Write body part of STL file.
                 * @param [in] File stream.
                 */
                bool readBody ( std::ifstream &fin ) {
                        Mesh& mesh = this->getMesh();
                        if ( this->isBinary() ) return this->read_body_binary ( fin, mesh ) ;
                        else                    return this->read_body_ascii  ( fin, mesh ) ;
                }
        public:
                /**
                 * @brief Return type of expoter.
                 * @return String value.
                 */
                virtual std::string toString ( void ) const {
                        return std::string ( "stl" );
                }


                bool read_body_binary ( std::ifstream& fin , Mesh & mesh ) {
                        for ( unsigned int i = 0 ; i < this->_num_faces ; i++ ) {
                                float nx, ny, nz;
                                fin.read ( ( char* ) & ( nx ), sizeof ( float ) );
                                fin.read ( ( char* ) & ( ny ), sizeof ( float ) );
                                fin.read ( ( char* ) & ( nz ), sizeof ( float ) );
                                for ( int j = 0 ; j < 3 ; j++ ) {
                                        float x, y, z;
                                        fin.read ( ( char* ) &x, sizeof ( float ) );
                                        fin.read ( ( char* ) &y, sizeof ( float ) );
                                        fin.read ( ( char* ) &z, sizeof ( float ) );
                                        Eigen::Vector3d v ( x,y,z );
                                        mesh.addPoint( v );
                                }
                                std::vector<int> idx;
                                idx.push_back( i * 3 + 0 ) ;
                                idx.push_back( i * 3 + 1 ) ;
                                idx.push_back( i * 3 + 2 ) ;
                                mesh.addFace( idx );

                                char buf[2];
                                fin.read ( buf, 2 );

                        }
                        return fin.good();
                }

                bool read_body_ascii (  std::ifstream& fin , Mesh & mesh ) {
                        std::cerr<<" stl (ascii) is not implemented yet."<<std::endl;
                        return false;
                }
        };
}
#endif// MI_MESH_IMPORTER_STL_HPP

