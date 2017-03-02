/**
 * @file MeshExporterOff.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_MESH_EXPORTER_OFF_HPP
#define MI_MESH_EXPORTER_OFF_HPP 1

#include <vector>
#include "MeshExporter.hpp"

namespace mi
{
        /**
         * @class MeshExporterOff MeshExporterOff.hpp <mi/MeshExporterOff.hpp>
         * @brief Off Exporter.
         */
        class  MeshExporterOff : public MeshExporter
        {
        public:
                /**
                 * @brief Constructor.
                 * @param [in] mesh Mesh offect.
                 */
                explicit MeshExporterOff ( const Mesh& mesh ) :  MeshExporter( mesh, false ) {
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                virtual ~MeshExporterOff ( void ) {
                        return;
                }
        private:
                /**
                 * @brief Write header part of STL file.
                 * @param [in] File stream.
                 */
                virtual bool writeHeader( std::ofstream &fout ) {
                        const Mesh& mesh = this->getMesh();
                        const int vnum = mesh.getNumVertices();
                        const int fnum = mesh.getNumFaces();
                        const int ednum = 0;
                        fout<<"OFF"<<std::endl;
                        fout<<vnum<<" "<<fnum<<" "<<ednum<<std::endl;
                        return fout.good();
                }
                /**
                 * @brief Write body part of STL file.
                 * @param [in] File stream.
                 */
                virtual bool writeBody ( std::ofstream &fout ) {
                        const Mesh& mesh = this->getMesh();
                        for ( int i = 0 ; i < mesh.getNumVertices() ; ++i ) {
                                Vector3d v0 = mesh.getPosition ( i );
                                fout<<v0.x()<<" "<<v0.y()<<" "<<v0.z()<<std::endl;
                        }

                        for ( int i = 0 ; i < mesh.getNumFaces() ; ++i ) {
                                std::vector<int> idx = this->getMesh().getFaceIndices( i );
                                fout<<idx.size();
                                for( size_t j = 0 ; j < idx.size(); ++j ) {
                                        fout<<" "<<idx[j];
                                }
                                fout<<std::endl;
                        }
                        return fout.good();
                }
        public:
                /**
                 * @brief Return type of expoter.
                 * @return String value.
                 */
                virtual std::string toString ( void ) const {
                        return std::string ( "off" );
                }
        };
}
#endif// MI_MESH_EXPORTER_OFF_HPP

