/**
 * @file MeshExporterPly.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_MESH_EXPORTER_PLY_WITH_COLOR_HPP
#define MI_MESH_EXPORTER_PLY_WITH_COLOR_HPP 1

#include <vector>
#include "MeshExporter.hpp"

namespace mi
{
        /**
         * @class MeshExporterPlyWithColor MeshExporterPlyWithColor.hpp <mi/MeshExporterPlyWithColor.hpp>
         * @brief PlyWithColor Exporter.
         */
        class  MeshExporterPlyWithColor : public MeshExporter
        {
        public:
                /**
                 * @brief Constructor.
                 * @param [in] mesh Mesh object.
                 */
                explicit MeshExporterPlyWithColor ( const Mesh& mesh, std::vector<Color3f>& col, bool isBinary = false ) :  MeshExporter( mesh, false ), _col( col ), _isBinaryPly( isBinary ) {
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                ~MeshExporterPlyWithColor ( void ) {
                        return;
                }
        private:
                /**
                 * @brief Write header part of STL file.
                 * @param [in] File stream.
                 */
                bool writeHeader( std::ofstream &fout ) {
                        if( !fout.is_open() ) return false;
                        fout<<"ply"<<std::endl;
                        if ( this->_isBinaryPly ) fout<<"format binary_little_endian 1.0"<<std::endl;
                        else fout<<"format ascii 1.0"<<std::endl;
                        fout<<"element vertex "<<this->getMesh().getNumVertices()<<std::endl;
                        fout<<"property float x"<<std::endl;
                        fout<<"property float y"<<std::endl;
                        fout<<"property float z"<<std::endl;
                        fout<<"property uchar red"<<std::endl;
                        fout<<"property uchar green"<<std::endl;
                        fout<<"property uchar blue"<<std::endl;
                        fout<<"element face "<<this->getMesh().getNumFaces()<<std::endl;
                        fout<<"property list uchar int vertex_index"<<std::endl;
                        fout<<"end_header"<<std::endl;
                        return fout.good();
                }
                /**
                 * @brief Write body part of STL file.
                 * @param [in] File stream.
                 */
                bool writeBody ( std::ofstream &fout ) {
                        const Mesh& mesh = this->getMesh();
                        for ( int i = 0 ; i < mesh.getNumVertices() ; ++i ) {
                                mi::Vector3d v = mesh.getPosition( i );
                                if ( !this->write_vertex( v, this->_col[i], fout ) ) return false;
                        }
                        for ( int i = 0 ; i < mesh.getNumFaces() ; ++i ) {
                                std::vector<int> idx = this->getMesh().getFaceIndices( i );
                                if ( !this->write_face_index( idx, fout ) ) return false;
                        }
                        return true;
                }
        public:
                /**
                 * @brief Return type of expoter.
                 * @return String value.
                 */
                virtual std::string toString ( void ) const {
                        if( this->_isBinaryPly ) return std::string ( "ply(binary)" );
                        else return std::string ( "ply(ascii)" );
                }
        private:
                bool write_vertex( mi::Vector3d& v, mi::Color3f& col, std::ofstream& fout  ) {
                        if ( this->_isBinaryPly ) {
                                float p[3];
                                p[0] = static_cast<float>( v.x() );
                                p[1] = static_cast<float>( v.y() );
                                p[2] = static_cast<float>( v.z() );
                                if( !fout.write( ( char* )( &p[0] ), sizeof( float ) * 3 ) ) return false;
                                unsigned char c[3];
                                c[0] = static_cast<unsigned char>( col.x() * 255 );
                                c[1] = static_cast<unsigned char>( col.y() * 255 );
                                c[2] = static_cast<unsigned char>( col.z() * 255 );
                                if( !fout.write( ( char* )( &c[0] ), sizeof( unsigned char ) * 3 ) ) return false;
                        } else {
                                fout<<v.x()<<" "<<v.y()<<" "<<v.z()<<" "
                                    <<static_cast<int>( col.x()*255 )<<" "<<static_cast<int>( col.y()*255 )<<" "<<static_cast<int>( col.z()*255 )<<std::endl;
                        }
                        return fout.good();
                }

                bool write_face_index ( std::vector<int>& idx, std::ofstream& fout ) {
                        if ( this->_isBinaryPly ) {
                                unsigned char n = static_cast<unsigned char>( idx.size() );
                                if( !fout.write( ( char* )( &n ), sizeof( unsigned char ) ) ) {
                                        return false;
                                }
                                if( !fout.write( ( char* )( &idx[0] ), sizeof( int ) * n ) ) {
                                        return false;
                                }
                        } else {
                                fout<<idx.size();
                                for ( size_t j = 0 ; j < idx.size() ; ++j ) {
                                        fout<<" "<<idx[idx.size() - 1 - j];
                                }
                                fout<<std::endl;
                        }
                        return fout.good();
                }
        private:
                std::vector<Color3f>& _col;
                bool _isBinaryPly;
        };
}
#endif// MI_MESH_EXPORTER_PLYWITHCOLOR_HPP

