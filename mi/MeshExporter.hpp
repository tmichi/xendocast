/**
 * @file MeshExpoter.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_MESH_EXPORTER_HPP
#define MI_MESH_EXPORTER_HPP 1
#include "Mesh.hpp"
#include "Exporter.hpp"
namespace mi
{
        class MeshExporter : public Exporter
        {
        protected:
                const Mesh& _mesh;
        protected:
                /**
                 * @brief Constructor.
                 * @param [in] mesh Mesh object.
                 * @param [in] isBinary Set true the file is binary format.
                 */
                explicit MeshExporter ( const mi::Mesh& mesh, const bool isBinary = true ) : Exporter ( isBinary ), _mesh ( mesh ) {
                        return;
                }
        public:
                /**
                 * @brief Destructor.
                 */
                virtual ~MeshExporter ( void ) {
                        return;
                }
        protected:
                /**
                 * @brief Get mesh instance.
                 * @return Mesh instance.
                 */
                const Mesh& getMesh ( void ) {
                        return this->_mesh;
                }
        };
}
#endif
