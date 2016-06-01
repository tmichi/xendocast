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
	MaskingRoutine ( const mi::VolumeData<char>& maskData, mi::VolumeData<T>& ctData ) : 
		mi::Routine ("masking"), _maskData(maskData), _ctData(ctData), _ctValue(0) {
		if ( const_cast<mi::VolumeData<char>& >(maskData).getSize() != ctData.getSize() ) {
			std::cerr<<"different size"<<std::endl;
			this->set_failed();
		}
		return;
	}
	
	MaskingRoutine& setCtValue( const float ctValue ) {
		this->_ctValue = ctValue;
		return *this;
	}

	bool run_main_routine ( void ) {
		const mi::VolumeData<char>& maskData = this->_maskData;
		const mi::VolumeInfo& info = const_cast<mi::VolumeData<char>&>( maskData ).getInfo();
		mi::Range range( info );
		for( auto&&p : range ) {
			// CT値が閾値以下 && マスクが0 
			const char mask = maskData.get(p);
			if ( mask == 2 ) continue; // skip endocast labels
			if(  this->_ctData.get(p) < this->_ctValue) {
				this->_ctData.set(p, this->_ctValue);
			}
		}
		return true;
	}
};
#endif// MASKING_ROUTINE_HPP

