/**
 * @file MeshExpoter.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_MESH_IMPORTER_HPP
#define MI_MESH_IMPORTER_HPP 1

#include "Mesh.hpp"
#include "Importer.hpp"

namespace mi
{
        class MeshImporter : public Importer
        {
        protected:
                Mesh& _mesh;
        protected:
                /**
                 * @brief Constructor.
                 * @param [in] mesh Mesh object.
                 * @param [in] isBinary Set true the file is binary format.
                 */
                explicit MeshImporter ( mi::Mesh& mesh, const bool isBinary = true ) : Importer ( isBinary ), _mesh ( mesh ) {
                        return;
                }
        public:
                /**
                 * @brief Destructor.
                 */
                virtual ~MeshImporter ( void ) {
                        return;
                }
        protected:
                /**
                 * @brief Get mesh instance.
                 * @return Mesh instance.
                 */
                Mesh& getMesh ( void ) {
                        return this->_mesh;
                }
        };
}
#endif
