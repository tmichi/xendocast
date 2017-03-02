/**
* @file AssemblyMesh.hpp
* @author Takashi Michikawa
*/
#pragma once
#ifndef __MI_ASSEMBLY_MESH_HPP__
#define __MI_ASSEMBLY_MESH_HPP__ 1
#include <algorithm>
#include <utility>
#include <deque>
#include <vector>
#include <stack>
#include "Mesh.hpp"
/**
* @class AssemblyMesh AssemblyMesh "mi/AssemblyMesh.hpp"
* @brief A set of meshes
*/
namespace mi
{
        class AssemblyMesh
        {
        private:
                std::deque<Mesh*> _mesh;
        private:
                AssemblyMesh ( const AssemblyMesh& mesh ) ;
                void operator = ( const AssemblyMesh& mesh ) ;
        public:
                /**
                * @brief Constructor.
                */
                explicit AssemblyMesh ( void ) {
                        this->init();
                        return;
                };
                /**
                * @brief Destructor
                */

                virtual ~AssemblyMesh ( void ) {
                        this->init();
                        return;
                };

                int create( void ) {
                        int newId = this->getNumMeshes();
                        this->_mesh.push_back( new Mesh() );
                        return newId;
                }


                /**
                * @brief Get the number of meshes
                * @return The number of meshes in the assembly file.
                */
                int getNumMeshes ( void ) const {
                        return static_cast<int>( this->_mesh.size() );
                }

                /**
                * @brief Get a part mesh in the assembly.
                * @param [in] id i-th mesh i takes 0 <= i < n where n denotes the number of mesh ( return value of AssemblyMesh::getNumMeshes())
                * @return Mesh object.
                */
                Mesh* getMesh ( const size_t id ) {
                        return this->_mesh.at ( id );
                }


                /**
                * @brief Remove all meshes in the assembly.
                */
                void init ( void ) {
                        for( int i = 0 ; i < this->getNumMeshes() ; ++i ) {
                                if ( this->_mesh[i] != NULL ) delete this->_mesh[i];
                                this->_mesh[i] = NULL;
                        }
                        this->_mesh.clear();
                }
        };
}
#endif
