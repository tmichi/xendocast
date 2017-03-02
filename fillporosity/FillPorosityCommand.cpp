#include "FillPorosityCommand.hpp"
#include <mi/VolumeDataUtility.hpp>
#include <mi/Neighbor.hpp>
#include <mi/FileNameConverter.hpp>
#include <mi/SystemInfo.hpp>
#include <mi/ConnectedComponentLabellerRle.hpp>
#include <mi/FunctionObject.hpp>
#include "ConstrainedMorphology.hpp"

template<typename T>
FillPorosityCommand<T>::FillPorosityCommand ( void ) : mi::CommandTemplate( "xendocast" )
{
        mi::AttributeSet& attrSet = this->getAttributeSet();
        mi::Point3i &size = this->_size;
        mi::Vector3d &pitch = this->_pitch;

        attrSet.createStringAttribute( "-i", this->_ct_file, "ct image" ).setMandatory();
        attrSet.createStringAttribute( "-o", this->_output_file, "mesh file" ).setMandatory();

        attrSet.createTripleNumericAttribute<int>( "-size", size.x(), size.y(), size.z(), "size of volume" ).setMin( 1, 1, 1 ).setMax(512, 512, 1024).setMandatory();
        attrSet.createTripleNumericAttribute<double>( "-pitch", pitch.x(), pitch.y(), pitch.z(), "pitch size" ).setMin ( 0.0001, 0.0001, 0.0001 ).setDefaultValue( 1,1,1 );
        attrSet.createNumericAttribute<int> ( "-h", this->_header_size, "header size" ).setDefaultValue( 0 ).setMin( 0 );

        attrSet.createNumericAttribute<int> ( "-thread", this->_num_threads, "The number of threads" ).setDefaultValue( mi::SystemInfo::getNumCores() ).setMin( 1 );
        attrSet.createNumericAttribute<double>( "-iso", this->_isovalue, "isovalue" ).setMandatory();
        attrSet.createNumericAttribute<double>( "-r", this->_radius, "Radius" ).setMandatory();
        
        attrSet.createNumericAttribute<int>("-ncp", this->_num_pruning, "The number of constrained pruning").setMin(0).setDefaultValue(0);
        attrSet.createNumericAttribute<int>("-ncg", this->_num_growing, "The number of constrained growing").setMin(0).setDefaultValue(0);

        return ;
}

template<typename T>
FillPorosityCommand<T>::~FillPorosityCommand ( void )
{
        return;
}

template<typename T>
bool
FillPorosityCommand<T>::init ( const mi::Argument& arg )
{
        if ( ! this->getAttributeSet().parse( arg ) ) return false;

        this->_ctData.init( mi::VolumeInfo( this->_size, this->_pitch ) );
        if ( ! mi::VolumeDataUtility::open( this->_ctData, this->_ct_file, this->_header_size ) ) return false;
        if ( this->isDebugModeOn() ) {
                mi::VolumeDataUtility::setDebugModeOn();
        }
        mi::VolumeDataUtility::setNumThread( this->_num_threads );
        return true;
}

template<typename T>
bool
FillPorosityCommand<T>::run  ( void )
{
	const mi::VolumeInfo& info = const_cast<mi::VolumeData<T>&>( this->_ctData ).getInfo();
        const mi::Point3i& size = info.getSize();
        // binarize
	mi::VolumeData<char> binaryData( info );
	std::cerr<<"binarization ";
	mi::VolumeDataUtility::binarize<T>( this->_ctData, binaryData, static_cast<T>(this->_isovalue) );
	std::cerr<<"done"<<std::endl;
        // fill hole
	mi::VolumeData<char> labelData( size );
	std::cerr<<"filling"<<std::endl;
        this->fill_closed_porosity( binaryData, labelData ) ;
	mi::VolumeDataUtility::debug_save( labelData, "labeldata.raw" );
	std::cerr<<"done"<<std::endl;

        // morphology
	mi::VolumeData<char> closeData( size );
	std::cerr<<"closing"<<std::endl;
        this->closing( labelData, closeData, this->_radius );
	mi::VolumeDataUtility::debug_save( closeData, "closedata.raw" );
	std::cerr<<"done"<<std::endl;
        // replacement
        for( int z = 0 ; z < size.z() ; ++z ) {
                for( int y = 0 ; y < size.y() ; ++y ) {
                        for( int x = 0 ; x < size.x() ; ++x ) {
                                const mi::Point3i p ( x,y,z );
                                if ( closeData.get( p ) == 1 && labelData.get( p ) == 0 ) { // the data is void
                                        this->_ctData.set( p, static_cast<T>(this->_isovalue) );
                                }
                        }
                }
        }
        return true;
}

template<typename T>
bool
FillPorosityCommand<T> ::term ( void )
{
        return mi::VolumeDataUtility::save( this->_ctData, this->_output_file );
}

/*
 ｓ*
 */
template<typename T>
void
FillPorosityCommand<T>::fill_closed_porosity (  mi::VolumeData<char>& inData, mi::VolumeData<char>& outData )
{
        const mi::Point3i& size = inData.getSize();
        for( int z = 0 ; z < size.z() ; ++z ) {
                for( int y = 0 ; y < size.y() ; ++y ) {
                        for( int x = 0 ; x < size.x() ; ++x ) {
                                const mi::Point3i p ( x,y,z );
                                const char value = 1 - inData.get( p );
                                inData.set( p, value );
                        }
                }
        }
        mi::VolumeDataUtility::extract_not_nth_components( inData, outData, 1 );
}


template<typename T>
void
FillPorosityCommand<T>::closing ( mi::VolumeData<char>& inData, mi::VolumeData<char>& outData , const double radius )
{
        mi::VolumeInfo& info = const_cast<mi::VolumeData<T>&>( this->_ctData ).getInfo();
        mi::VolumeData<char> tmpData0( info );
        mi::VolumeDataUtility::dilate( inData, tmpData0, radius );
        mi::VolumeData<char> tmpData1( info );
        mi::VolumeDataUtility::extract_nth_component ( tmpData0, tmpData1, 1 );
        mi::VolumeDataUtility::erode ( tmpData1, outData, radius );
        return;
}

template<typename T>
void 
FillPorosityCommand<T>::constrained_opening ( mi::VolumeData<char>& inData, mi::VolumeData<char>& maskData, const int num_pruning, const int num_growing) {
        ConstrainedMorphology morphology(inData, maskData);
        std::cerr<<"pruning ";
        for ( int i = 0 ; i < num_pruning ; ++i ) {
                morphology.prune();
                std::cerr<<".";
        }
        std::cerr<<std::endl;
        std::cerr<<"growing ";
        for ( int i = 0 ; i < num_growing ; ++i ) {
                morphology.grow();
                std::cerr<<".";
        }
        std::cerr<<std::endl;
        return;
}

template class FillPorosityCommand<char>;
template class FillPorosityCommand<unsigned char>;
template class FillPorosityCommand<short>;
template class FillPorosityCommand<unsigned short>;
template class FillPorosityCommand<int>;
template class FillPorosityCommand<unsigned int>;
template class FillPorosityCommand<float>;
template class FillPorosityCommand<double>;









