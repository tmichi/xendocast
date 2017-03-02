/**
 * @file MeshExporterStl.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_MESH_EXPORTER_STL_HPP
#define MI_MESH_EXPORTER_STL_HPP 1

#include <vector>
#include <iostream>
#include "MeshExporter.hpp"

namespace mi
{
        /**
         * @class MeshExporterStl MeshExporterStl.hpp <mi/MeshExporterStl.hpp>
         * @brief Mesh Expoter as STL format.
         */
        class  MeshExporterStl : public MeshExporter
        {
        private:
                std::vector<unsigned short> _property; //< Property data (2bytes/face).
        public:
                /**
                 * @brief Constructor.
                 * @param [in] mesh Mesh object.
                 */
                explicit MeshExporterStl ( const Mesh& mesh ) :  MeshExporter( mesh, true ) {
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                virtual ~MeshExporterStl ( void ) {
                        return;
                }
                /**
                 * @brief Add properties to faces.
                 * @param [in] prop Property value.
                 * @return Instance itself.
                 */
                MeshExporterStl& addProperty( std::vector<unsigned short>& prop ) {
                        this->_property.clear();
                        this->_property.insert( this->_property.end(), prop.begin(), prop.end() );
                        return *this;
                }
        private:
                /**
                 * @brief Write header part of STL file.
                 * @param [in] File stream.

                 */
                virtual bool writeHeader( std::ofstream &fout ) {
                        if( !fout.is_open() ) return false;
                        std::string name = this->getMesh().getName();
                        char ch[80];
                        //std::strcpy( ch, name.c_str() );
                        for( std::string::size_type i = 0 ; i < name.length() ; ++i ) {
                                if ( i == 80 ) break;
                                ch[i] = name[i];
                        }
                        unsigned int nf = static_cast<unsigned int>( this->getMesh().getNumFaces() );
                        if( !fout.write ( ch, 80 ) ) return false;
                        if( !fout.write ( ( char* )( &nf ), 4 ) ) return false;
                        return fout.good();
                }
                /**
                 * @brief Write body part of STL file.
                 * @param [in] File stream.
                 */
                virtual bool writeBody ( std::ofstream &fout ) {
                        const Mesh& mesh = this->getMesh();
                        for ( int i = 0 ; i < mesh.getNumFaces() ; ++i ) {
                                std::vector<int> idx = this->getMesh().getFaceIndices ( i );
                                Vector3d v0 = mesh.getPosition ( idx[0] );
                                Vector3d v1 = mesh.getPosition ( idx[1] );
                                Vector3d v2 = mesh.getPosition ( idx[2] );
                                Vector3d n  = mesh.getNormal( i, true );
                                std::vector<float> buf;
                                buf.reserve( 16 );
                                buf.push_back( static_cast<float>(  n.x() ) );
                                buf.push_back( static_cast<float>(  n.y() ) );
                                buf.push_back( static_cast<float>(  n.z() ) );
                                buf.push_back( static_cast<float>( v0.x() ) );
                                buf.push_back( static_cast<float>( v0.y() ) );
                                buf.push_back( static_cast<float>( v0.z() ) );
                                buf.push_back( static_cast<float>( v1.x() ) );
                                buf.push_back( static_cast<float>( v1.y() ) );
                                buf.push_back( static_cast<float>( v1.z() ) );
                                buf.push_back( static_cast<float>( v2.x() ) );
                                buf.push_back( static_cast<float>( v2.y() ) );
                                buf.push_back( static_cast<float>( v2.z() ) );
                                if( !fout.write ( ( char* )( &buf[0] ), 48 )  ) return false;
                                unsigned short s = ( i < static_cast<int>( this->_property.size() ) ) ? this->_property[i] : 0x0000;
                                if( !fout.write ( ( char* )( &s ), 2 ) ) return false;
                        }
                        return true;
                }
        public:
                /**
                 * @brief Return type of expoter.
                 * @return String value.
                 */
                virtual std::string toString ( void ) const {
                        return std::string ( "stl(binary)" );
                }
        };
}
#endif// MI_MESH_EXPORTER_STL_HPP
