#include "ConstrainedMorphology.hpp"
#include <mi/ParallelFor.hpp>
#define BG_VALUE 0
#define FG_VALUE 1

ConstrainedMorphology::ConstrainedMorphology (mi::VolumeData<char>& data, const mi::VolumeData<char>& mask ) : _data(data), _mask(mask) {
        return;
}

ConstrainedMorphology::~ConstrainedMorphology ( void ) {
        return;
}

bool 
ConstrainedMorphology::prune ( void ) {
       return this->morphology(BG_VALUE, FG_VALUE);
}


bool 
ConstrainedMorphology::grow ( void ) {
	return this->morphology(FG_VALUE, BG_VALUE);
}

bool 
ConstrainedMorphology::morphology (const char fgValue, const char bgValue ) {
        mi::VolumeInfo& info = this->_data.getInfo();
        mi::VolumeData<char> tmp(info);
        ConstrainedMorphology::morphology_fn fn(this->_data, this->_mask, tmp, fgValue, bgValue);
        mi::parallel_for_each(info.begin(), info.end(), fn);
        this->_data.clone(tmp);
        return true;
}
