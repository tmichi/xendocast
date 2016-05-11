#ifndef BINARIZATION_ROUTINE_HPP
#define BINARIZATION_ROUTINE_HPP 1
#include <iostream>
#include <memory>
#include <mi/Routine.hpp>
#include <mi/Range.hpp>
#include <mi/VolumeDataUtility.hpp>

template <typename T>
class BinarizationRoutine: public mi::Routine {
private:
	const mi::VolumeData<T>& _ctData;
	mi::VolumeData<char>& _binaryData;
	T _threshold;
public:
	BinarizationRoutine ( const mi::VolumeData<T>& ctData, mi::VolumeData<char>& binaryData ) : 
		mi::Routine ("binarization"), _ctData(ctData), _binaryData(binaryData), _threshold(T()) {
		return;
	}

	~BinarizationRoutine ( void ) {
		
		return;
	}

	BinarizationRoutine& setThreshold ( const T& threshold) {
		this->_threshold = threshold;
		return *this;
	}

	bool run_main_routine ( void ) {
		const mi::VolumeData<T>& ctData = this->_ctData;
		mi::VolumeData<char>&    binaryData  = this->_binaryData;
		mi::VolumeData<char>    tmpData(binaryData.getInfo());
	
		const T threshold = this->_threshold;
		// simple binarization
		mi::VolumeDataUtility::binarize<T, char>(ctData, tmpData, threshold);

		// Extract largest component.
		mi::VolumeDataUtility::label_rle_nth_component( tmpData, binaryData, 1 );
		mi::VolumeDataUtility::debug_save(this->create_temp_file_name ("b.raw"), binaryData);	
		mi::VolumeDataUtility::invert(binaryData);
		return true;
	}
};
#endif// BINARIZATION_ROUTINE_HPP


