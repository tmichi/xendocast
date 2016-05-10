#ifndef BINARIZATION_ROUTINE_HPP
#define BINARIZATION_ROUTINE_HPP 1

#include <mi/Routine.hpp>
#include <mi/Range.hpp>
#include <mi/VolumeDataUtility.hpp>
#include <iostream>
#include <memory>

template <typename T>
class BinarizationRoutine: public mi::Routine {
private:
	const mi::VolumeData<T>& _ctData;
	mi::VolumeData<char>& _binaryData;
	T _threshold;
public:
	BinarizationRoutine ( const mi::VolumeData<T>& ctData, mi::VolumeData<char>& binaryData ) : 
		mi::Routine ("binarization"), _ctData(ctData), _binaryData(binaryData), _threshold(T()) {
		// check validity here
		return;
	}
	
	BinarizationRoutine& setThreshold ( const T& threshold) {
		this->_threshold = threshold;
		return *this;
	}

	bool run_main_routine ( void ) {
		// simple binarization
		mi::VolumeDataUtility::binarize(this->_ctData, this->_binaryData, this->_isovalue);
		// Extract largest component.
		mi::VolumeDataUtility::extract_nth_component( tmpData, binaryData, 1 );
		// Invert 
		mi::VolumeDataUtility::invert(binaryData);
		return true;
	}
};
#endif// BINARIZATION_ROUTINE_HPP


