/**
 * @file Graph.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_GRAPH_HPP
#define MI_GRAPH_HPP 1
#include <deque>
#include <vector>
#include <queue>
namespace mi
{
        class Graph
        {
        private:
                class Node;
                Graph ( const Graph& that );
                void operator = ( const Graph& that );
        public:
                /**
                 * @brief Constructor.
                 * @param[in] numNodes The number of nodes.
                 */
                explicit Graph ( const int numNodes ) {
                        for( int i = 0 ; i < numNodes ; ++i ) {
                                this->addNode();
                        }
                        return ;
                }

                virtual ~Graph ( void ) {
                        return ;
                }

                int addNode ( void ) {
                        const int newId = static_cast<int>( this->_nodes.size() );
                        this->_nodes.push_back( Node( newId ) );
                        return newId;
                }

                void addEdge( const int v0, const int v1 ) {
                        if ( v0 == v1 ) return;
                        if ( v0 <   0 ) return;
                        if ( v1 <   0 ) return;
                        this->_nodes[v0].addConnectedVertex( v1 );
                        this->_nodes[v1].addConnectedVertex( v0 );
                        return;
                }

                int label ( void ) {
                        int numLabels = 0;
                        const int size = static_cast<int>( this->_nodes.size() );

                        for( int i = 0 ; i < size ; ++i ) {
                                std::sort( this->_nodes[i].getNeighbor().begin(), this->_nodes[i].getNeighbor().end() );
                        }

                        for( int i = 0 ; i < size ; ++i ) {
                                if( this->_nodes[i].getClusterId() < i ) continue; // cluster is already labelled.

                                const int newLabelId = numLabels;
                                this->_nodes[i].setClusterId( newLabelId );

                                std::queue<int> q;
                                q.push( i );
                                while( !q.empty() ) {
                                        const int front = q.front();
                                        q.pop();

                                        std::vector<int>& neighbor = this->_nodes[front].getNeighbor();
                                        for( std::vector<int>::iterator iter = neighbor.begin() ; iter != neighbor.end() ; ++iter ) {
                                                const int j = *iter;
                                                if( j == i ) continue;
                                                if( this->_nodes[j].getClusterId() == j ) {
                                                        this->_nodes[j].setClusterId( newLabelId );
                                                        q.push( j );
                                                }
                                        }
                                }
                                ++numLabels;
                        }
                        return numLabels;
                }

                int getClusterId( const int nodeId ) const {
                        if( nodeId < 0 || static_cast<int>( this->_nodes.size() ) <= nodeId ) {
                                std::cerr<<nodeId<<" is invalid id"<<std::endl;
                                return -1;
                        }
                        return this->_nodes[nodeId].getClusterId();
                }

                bool isConnected ( const int v0, const int v1 ) {
                        std::vector<int>& neighbor = this->_nodes[v0].getNeighbor();
                        //@todo should be optimized.
                        for( std::vector<int>::const_iterator iter = neighbor.begin() ; iter != neighbor.end() ; ++iter ) {
                                if(  *iter == v1 ) return true;
                        }
                        return false;
                }
        private:
                class Node
                {
                private:
                        int _clusterId;
                        std::vector<int> _neighbor;
                private:
                        void operator = ( const Node& that );
                public:
                        explicit Node( const int id ) : _clusterId( id ) {
                                return ;
                        }

                        Node ( const Node& that ) : _clusterId( that._clusterId ), _neighbor( that._neighbor ) {
                                return ;
                        }

                        virtual ~Node( void ) {
                                return ;
                        }

                        void addConnectedVertex( const int v ) {
                                for( std::vector<int>::iterator iter = this->_neighbor.begin() ; iter != this->_neighbor.end() ; ++iter ) {
                                        if ( *iter == v ) return; // skip when registered id is given .
                                }
                                this->_neighbor.push_back( v );
                                return;
                        }

                        void setClusterId ( const int id ) {
                                this->_clusterId = id;
                                return;
                        }

                        int getClusterId ( void ) const {
                                return this->_clusterId;
                        }

                        std::vector<int>& getNeighbor( void ) {
                                return this->_neighbor;
                        }
                };
                std::deque<Node> _nodes;
        };
}
#endif //MI_GRAPH_HPP
