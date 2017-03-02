#include "PorosityAnalyzer.hpp"
#include <mi/VolumeDataUtility.hpp>
#include <mi/Neighbor.hpp>
#include <mi/FileNameConverter.hpp>
#include <mi/SystemInfo.hpp>
#include "ConstrainedMorphology.hpp"
PorosityAnalyzer::PorosityAnalyzer ( const mi::VolumeData<char>& data, const double radius, const int num_pruning, const int num_growing) 
	: _data(data),  _radius(radius), _num_pruning(num_pruning), _num_growing ( num_growing)  {
	this->_porosity.init( const_cast<mi::VolumeData<char>&>(data).getInfo() );
	return;
}

PorosityAnalyzer::~PorosityAnalyzer ( void ) {
	return;
}

int 
PorosityAnalyzer::analyze ( void ) {
	mi::VolumeInfo& info = const_cast< mi::VolumeData<char>& >(this->_data).getInfo();
	mi::VolumeData<char> closedData(info), voidData(info);
	this->close_porosity( this->_data, closedData, this->_radius);
	this->remove_false_porosity (closedData, voidData, this->_num_pruning, this->_num_growing);
	this->classify_porosity ( voidData, this->_data, this->_porosity);
	return 1;
}


int 
PorosityAnalyzer::getPorosity ( mi::VolumeData<char>& porosityData , unsigned char detected_types , bool isBinarized) {
	std::vector<char> values(4, 0) ;
	if ( POROSITY_BG | detected_types ) {
		values[0] = 0 ;
		if ( isBinarized ) values [0] = 1;
	}
	if ( POROSITY_CLOSED | detected_types ) {
		values[1] = 1 ;
		if ( isBinarized ) values[1] = 1;
	}
	if ( POROSITY_OPEN | detected_types ) {
		values[2] = 3 ;
		if ( isBinarized ) values[2] = 1;
	}
	if ( POROSITY_FG | detected_types ) {
		values[3] = 4;
		if ( isBinarized ) values[3] = 1;
	}
	porosityData.clone(this->_porosity);
	mi::parallel_for_each( porosityData.begin(), porosityData.end(), mi::look_up<char>(values));
	return 1;
}
template<typename T>
int 
PorosityAnalyzer::getReplacedVolume ( mi::VolumeData<T>& data, const T value ) {
	data.clone(this->_porosity);
	const mi::Point3i& size = data.getInfo().getSize();
	for ( int z = 0 ; z < size.z(); ++z ){
		for ( int y = 0 ; y < size.y(); ++y ) {
			for( int x = 0 ; x < size.x() ; ++x) {
				switch ( this->_porosity.get(x,y,z) ) {
				case 1:
				case 2:
					data.set(x, y, z, value);
					break;
				}
			}
 		}
	}
	return 1;
}

int 
PorosityAnalyzer::close_porosity( const mi::VolumeData<char>& inData, mi::VolumeData<char>& outData, const double radius){
	const mi::VolumeInfo& info = const_cast<mi::VolumeData<char>&>(inData).getInfo();
	mi::VolumeData<char> tmpData( info );

	mi::VolumeDataUtility::negate_binary(inData, outData);
	mi::VolumeDataUtility::extract_not_nth_components( outData, tmpData, 1 );
	mi::VolumeDataUtility::dilate( tmpData, outData, radius );
	mi::VolumeDataUtility::extract_nth_component ( outData, tmpData, 1 );
	mi::VolumeDataUtility::erode ( tmpData, outData, radius );
	return 1;
}

int 
PorosityAnalyzer::remove_false_porosity (mi::VolumeData<char>& inData, mi::VolumeData<char>& maskData, const int np, const int ng) {
	ConstrainedMorphology morphology(inData, maskData);
	std::cerr<<"pruning ";
	for ( int i = 0 ; i < np ; ++i ) {
		morphology.prune();
		std::cerr<<".";
	}
	std::cerr<<std::endl;
	std::cerr<<"growing ";
	for ( int i = 0 ; i < ng ; ++i ) {
		morphology.grow();
		std::cerr<<".";
	}
	std::cerr<<std::endl;
	return 1;
}
int 
PorosityAnalyzer::classify_porosity ( const mi::VolumeData<char>& inData, const mi::VolumeData<char>& maskData, mi::VolumeData<char>& outData) {
	mi::VolumeInfo& info = const_cast< mi::VolumeData<char>& >(inData).getInfo();
	mi::VolumeData<char> tmpData(info);
	mi::VolumeDataUtility::diff(inData, maskData, tmpData);
	int nlabels = 0;
	const char CLOSED_POROSITY = 1;
	const char OPEN_POROSITY =   3;
	std::vector<char> types( nlabels, POROSITY_BG);
	// 分離するルーチン	
	mi::VolumeData<int> labelData(info);
	mi::VolumeDataUtility::label_rle( tmpData, labelData, 6, false, &nlabels);
	return 1;
}

/*

[ヘッダ(16バイト)]
0   : 'VOID' (char 4bytes)
4   : int (X）
8   : int (Y)
12  : int (Z)

[実部]
各ボクセルが持つ値は，以下の通りである．
0 (000): 背景ボクセル．
1 (001): Closed voidボクセル．
3 (011): Open voidボクセル.
4 (100): 前景ボクセル．

*/
