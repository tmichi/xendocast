#ifndef WATERSHED_ROUTINE_HPP
#define WATERSHED_ROUTINE_HPP 1

#include <mi/Routine.hpp>
#include <mi/Range.hpp>
#include <mi/Clamp.hpp>
#include <mi/VolumeDataUtility.hpp>
#include <mi/VolumeDataCreator.hpp>
#include <iostream>
#include <memory>

class WatershedRoutine: public mi::Routine {
private:
	const mi::VolumeData<char>& _binaryData;
	mi::VolumeData<char>& _labelData;
	float _hole_size;
	float _scale;
public:
	WatershedRoutine ( const mi::VolumeData<char>& binaryData, mi::VolumeData<char>& labelData ) : 
		mi::Routine ("Watershed"), _binaryData(binaryData), _labelData(labelData), _holeSize(-1), _scale(0.5) {
		// check validity here
		return;
	}
	
	WatershedRoutine& setHoleSize( const float holeSize ) {
		this->_hole_size = holeSize;
		return *this;
	}

	WatershedRoutine& setScale( const float scale ) {
		this->_scale = mi::clamp(scale, 1.0e-4, 1);
		return *this;
	}

	

	bool run_main_routine ( void ) {
		mi::VolumeData<float> distData;
		mi::VolumeDataUtility::compute_distance_field(this->_binaryData, distData);
		
		// init label
		mi::VolumeData<char> labelData;
		if ( this->is_automatic() ) {
			this->init_label_automatic(distData, labelData);
		}
		else {
			this->init_label_manual(distData, labelData, this->_hole_size);
		}
		
		// Watershed 
		this->watershed ( distData, labelData) ; 

		return true;
	}

private:
	bool is_automatic ( void ) const {
		return this->_hole_size < 0 ;
	}

	void init_label_automatic ( const mi::VolumeData<float>& distdata, mi::volumeData<char>& initData ) {
                std::cerr<<"automatic mode."<<std::endl;
                const float& scale = this->_scale;
		mi::VolumeInfo& info = distData.getInfo();
		mi::Range range( info.getMin(), info.getMax() );
		
		mi::VolumeDataCreator<char> creator( initData ) ;

		// Corner voxels
                creator.setValue( 1 ); // other voxel
                for( mi::Range::iterator iter = range.begin() ; iter != range.end() ; ++iter ) {
                        const mi::Point3i& p = *iter;
                        //if ( info.isCorner( p ) && labelData.get( p ) == 0 ) { // speed up
                        if ( info.isCorner( p ) ) {
                                const float dist = distData.get( p );
                                if ( dist > 0 ) creator.fillSphere( p, dist * scale );
                        }
                }
		
		// finding center endocranial voxel 
                mi::Point3i maxp  ;
                float maxDist = 0;
                for( mi::Range::iterator iter = range.begin() ; iter != range.end() ; ++iter ) {
                        const mi::Point3i& p = *iter;
                        if ( labelData.get( p ) == 1 ) continue; // skip other voxels.
                        const float dist = distData.get( p );
                        if ( maxDist < dist ) {
                                maxDist = dist;
                                maxp = p;
                        }
                }
                creator.setValue( 2 );
                creator.fillSphere( maxp, maxDist * scale ); // 0.5

		return;
	}

	void init_label_manual ( const mi::VolumeData<float>& distdata, mi::volumeData<char>& initData , const float holeSize ) {
		mi::VolumeInfo& info = distData.getInfo();
		mi::Range range( info.getMin(), info.getMax() );
		mi::VolumeData<char> binaryData( info );
		initData.init(info);
		for( mi::Range::iterator iter = range.begin() ; iter != range.end() ; ++iter ) {
                        const mi::Point3i p = *iter;
                        const char value = ( distData.get( p ) < holesize ) ? 0 : 1; //
                        binaryData.set( p,value );
                }
                
		/*
		 * @todo should be considered in case of more than three components exist.
		 */
		// 0 : fg
		// 1 : other voxel 
	        // 2 : endcranial voxels
		// 3~ : others.
                int numLabel = 0;
                if( !mi::VolumeDataUtility::label_rle( binaryData, initData, 26, true, &numLabel ) ) return false;
		return;
	}

	void watershed ( mi::VolumeData<float>& distData, mi::VolumeData<char>& labelData) {
		const mi::VolumeInfo& info = const_cast<mi::VolumeData<T>&>( labelData ).getInfo();
		mi::Range range( info.getMin(), info.getMax() );

		mi::PriorityQueue <Point3i> pq;
		
		for( mi::Range::iterator iter = range.begin() ; iter != range.end() ; ++iter ) {
			const mi::Point3i& p = *iter;
			if ( labelData.get( p ) > 0 ) {
				pq.push( p, -this->_weightData.get( p ) ) ;
			}
		}
		
		while( !pq.empty() ) {
			const Point3i p = pq.getTopIndex();
			pq.pop();

			const int labelId = labelData.get( p ); // Label ID to be propagated.

			for( mi::Neighbor::iterator diter = mi::Neighbor::begin() ; diter != mi::Neighbor::end( 6 ) ; ++diter ) {
				const mi::Point3i np = *diter + p;
				if ( !info.isValid( np ) ) continue;
				if ( labelData.get( np ) != 0 ) continue; // skip bg

				if( this->_weightData.get( np ) > 0 ) { // propagate only to bg voxels.
					labelData.set( np, labelId );
					pq.push( np, -this->_weightData.get( np ) );
				}
			}
		}
 		return true;
	}
};
#endif// WATERSHED_ROUTINE_HPP

