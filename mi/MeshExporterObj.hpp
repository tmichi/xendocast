/**
* @file MeshExporterObj.hpp
* @author Takashi Michikawa <michikawa@acm.org>
*/
#ifndef MI_MESH_EXPORTER_OBJ_HPP
#define MI_MESH_EXPORTER_OBJ_HPP 1

#include <vector>
#include "MeshExporter.hpp"

namespace mi
{
        /**
         * @class MeshExporterObj MeshExporterObj.hpp <mi/MeshExporterObj.hpp>
         * @brief Obj Exporter.
         */
        class  MeshExporterObj : public MeshExporter
        {
        public:
                /**
                 * @brief Constructor.
                 * @param [in] mesh Mesh object.
                 */
                explicit MeshExporterObj ( const Mesh& mesh ) :  MeshExporter( mesh, false ) {
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                virtual ~MeshExporterObj ( void ) {
                        return;
                }
        private:
                /**
                 * @brief Write body part of STL file.
                 * @param [in] File stream.
                 */
                virtual bool writeBody ( std::ofstream &fout ) {
                        const Mesh& mesh = this->getMesh();
                        for ( int i = 0 ; i < mesh.getNumVertices() ; ++i ) {
                                Vector3d v0 = mesh.getPosition ( i );
                                fout<<"v "<<v0.x()<<" "<<v0.y()<<" "<<v0.z()<<std::endl;
                        }

                        for ( int i = 0 ; i < mesh.getNumFaces() ; ++i ) {
                                std::vector<int> idx = this->getMesh().getFaceIndices( i );
                                fout<<"f";
                                for ( size_t j = 0 ; j < idx.size() ; ++j ) {
                                        fout<<" "<<idx[idx.size() - 1 - j] + 1; //starts from 1.
                                }
                                fout<<std::endl;
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
#endif// MI_MESH_EXPORTER_OBJ_HPP

