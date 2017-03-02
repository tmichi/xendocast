/**
 * @file ConnectedComponentLabeller.hpp
 * @author Takashi Michkawa <michikawa@acm.org>
 */
#pragma once
#ifndef MI_CONNECT_COMPONENT_LABELLER_HPP
#define MI_CONNECT_COMPONENT_LABELLER_HPP 1
#include <queue>
#include <iostream>

#include "ParallelFor.hpp"
#include "math.hpp"
#include "VolumeData.hpp"
#include "Neighbor.hpp"
#include "Limits.hpp"

namespace mi
{
        class ConnectedComponentLabeller
        {
        private:
                ConnectedComponentLabeller( const ConnectedComponentLabeller& that ) ;
                void operator = ( const ConnectedComponentLabeller& that ) ;
        private:
                const static int UNLABELLED = -2; ///< Unlabelled label.
                const static int BACKGROUND = 0;  ///< Background label.
                /**
                 * @brief Queue-based labelling algorithm.
                 */
                class ccl_queue
                {
                private:
                        /**
                         * @brief Initializer of labeling.
                         */
                        class ccl_init
                        {
                        private:
                                mi::VolumeData<char>& _binaryData;
                                mi::VolumeData<int>&  _labelData;
                                const bool _isFg;
                        public:
                                ccl_init( mi::VolumeData<char>& binaryData, mi::VolumeData<int>& labelData, const bool isFg = true ) :
                                        _binaryData( binaryData ), _labelData( labelData ), _isFg( isFg ) {
                                        return;
                                }

                                void operator () ( const mi::Point3i& p ) {
                                        this->_labelData.at( p ) = ( ( this->_binaryData.at( p ) != 0 ) == this->_isFg ) ? UNLABELLED : BACKGROUND;
                                        return;
                                }
                        };

                        mi::VolumeData<int>& _labelData;
                        int _labelId;
                        int _neighbor;
                public:
                        ccl_queue( mi::VolumeData<char>& binaryData, mi::VolumeData<int>& labelData, const int neighbor = 26, const bool isFg = true , const int grainSize = 800000 ) :
                                _labelData( labelData ) , _neighbor ( neighbor ) {
                                mi::VolumeInfo& info = binaryData.getInfo();
                                mi::parallel_for_each( info.begin(), info.end(), ccl_init( binaryData, this->_labelData, isFg ), grainSize );
                                this->_labelId = 0;
                                return;
                        }

                        int operator () ( const mi::Point3i& p ) {
                                if( this->_labelData.get( p ) == UNLABELLED ) {
                                        static mi::Neighbor nbr;
                                        this->_labelId += 1;
                                        std::queue<mi::Point3i> q;
                                        this->_labelData.set( p, this->_labelId );
                                        q.push( p );
                                        while ( !q.empty() ) {
                                                const mi::Point3i p0 = q.front();
                                                q.pop();
                                                for( mi::Neighbor::iterator iter = nbr.begin() ; iter != nbr.end( this->_neighbor ) ; ++iter ) {
                                                        const mi::Point3i p1 = p0 + ( *iter );
                                                        if ( !this->_labelData.getInfo().isValid( p1 ) ) continue;
                                                        if (  this->_labelData.get( p1 ) != UNLABELLED ) continue;
                                                        this->_labelData.set( p1, this->_labelId );
                                                        q.push( p1 );
                                                }
                                        }
                                }
                                return _labelId;

                        }
                        int getNumLabel( void ) {
                                return _labelId;
                        }
                };
        private:
                mi::VolumeData<char> &_data;
        public:

                explicit ConnectedComponentLabeller ( mi::VolumeData<char>& data ) : _data( data ) {
                        return;
                }
                ~ConnectedComponentLabeller ( void ) {
                        return;
                }

                /**
                 * @brief Apply labeling.
                 * @param [out] labelData Labelling result.
                 * @param [in] nbr Neighbor to be considered. nbr = 6, 18 or 26(default).
                 * @param [in] isSorted When this is set to true, Labeling result is sorted by a number of voxels in a descent manner.
                 * @return The number of labels.
                 */
                int apply ( mi::VolumeData<int>& labelData, const int nbr = 26, const bool isSorted = false ) {
                        return this->label ( labelData, nbr, isSorted );
                }
                /**
                 * @brief Apply labeling.
                 * @param [out] labelData Labelling result.
                 * @param [in] nbr Neighbor to be considered. nbr = 6, 18 or 26(default).
                 * @param [in] isSorted When this is set to true, Labeling result is sorted by a number of voxels in a descent manner.
                 * @return The number of labels.
                 */
                int label ( mi::VolumeData<int>& labelData, const int nbr = 26, const bool isSorted = false ) {
                        mi::VolumeInfo& info = this->_data.getInfo();
                        const int numLabels = std::for_each( info.begin(), info.end(), ccl_queue( this->_data, labelData, nbr, true ) ).getNumLabel();

                        if( isSorted ) {
                                std::vector<std::pair<int, int> > nextLabel;
                                for ( int i = 0 ; i <= numLabels ; ++i ) {
                                        nextLabel.push_back( std::make_pair( 0, i ) );
                                }

                                for( mi::VolumeInfo::iterator iter = info.begin() ; iter != info.end() ; ++iter ) {
                                        nextLabel[ labelData.get( *iter ) ].first += 1;
                                }
                                nextLabel[0].first = mi::max_value<int>();

                                // sort by comp. num.
                                std::sort( nextLabel.begin(), nextLabel.end(), std::greater<std::pair<int, int> >() );

                                for( int i = 0 ; i <= numLabels ; i++ ) {
                                        nextLabel[i].first = nextLabel[i].second;
                                        nextLabel[i].second = i;
                                }
                                // sort by label id
                                std::sort( nextLabel.begin(), nextLabel.end() );

                                for( mi::VolumeInfo::iterator iter = info.begin() ; iter != info.end() ; ++iter ) {
                                        const mi::Point3i p = *iter;
                                        const int oldLabel = labelData.get( p );
                                        labelData.set( p , nextLabel[ oldLabel ].second );
                                }
                        }
                        return numLabels;
                };
        };
}

#endif// MI_CONNECT_COMPONENT_LABELLER_HPP
