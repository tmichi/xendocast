#ifndef MASKING_ROUTINE_HPP
#define MASKING_ROUTINE_HPP 1

#include <mi/Routine.hpp>
#include <mi/Range.hpp>
#include <mi/Clamp.hpp>
#include <mi/VolumeDataUtility.hpp>
#include <mi/VolumeDataCreator.hpp>
#include <iostream>
#include <memory>

template <typename T>
class MaskingRoutine: public mi::Routine {
private:
	const mi::VolumeData<char>& _maskData;
	mi::VolumeData<T>& _ctData;
	float _ctValue;
public:
	MaskingRoutine ( const mi::VolumeData<char>& maskData, mi::VolumeData<T>& ctData , const float ctValue) : 
		mi::Routine ("masking"), _maskData(maskData), _ctData(ctData), _ctValue(cvValue) {
		// check validity here
		if ( maskData.getSize() != ctData.getSize() ) {
			this->set_failed();
		}
		return;
	}
	
	MaskingRoutine& setCtValue( const float ctValue ) {
		this->_ctValue = ctValue;
		return *this;
	}

	bool run_main_routine ( void ) {
		const mi::VolumeInfo& info = const_cast<mi::VolumeData<T>&>( labelData ).getInfo();
		mi::Range range( info.getMin(), info.getMax() );
		
		//
		for( mi::Range::iterator iter = range.begin() ; iter != range.end() ; ++iter ) {
			const mi::Point3i& p = *iter;
			if ( maskData.get( p ) == 2 ) {
				this->_ctData.set(p, this->_ctValue);
			}
		}
		return true;
	}
};
#endif// MASKING_ROUTINE_HPP

