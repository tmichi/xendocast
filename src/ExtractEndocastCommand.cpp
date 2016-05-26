#include "ExtractEndocastCommand.hpp"
#include <mi/VolumeDataUtility.hpp>

#include "BinarizationRoutine.hpp"
#include "WatershedRoutine.hpp"

#include <mi/FileNameConverter.hpp>
#include <mi/SystemInfo.hpp>

#include <mi/VolumeDataPolygonizer.hpp>
#include <mi/TriMeshExporterObj.hpp>

template<typename T>
ExtractEndocastCommand<T>::ExtractEndocastCommand ( void ) : mi::CommandTemplate( "xendocast" )
{
        mi::AttributeSet& attrSet = this->getAttributeSet();
        attrSet.createStringAttribute( "-i", this->_ct_file, "ct image" ).setMandatory();
        attrSet.createStringAttribute( "-o", this->_output_file, "mesh file" ).setMandatory();

        mi::Point3i &size = this->_size;
        mi::Vector3d &pitch = this->_pitch;
        mi::Vector3d &origin = this->_origin;

        attrSet.createTripleNumericAttribute<int>( "-size", size.x(), size.y(), size.z(), "size of volume" ).setMin( 1, 1, 1 ).setMandatory();
        attrSet.createNumericAttribute<int> ( "-h", this->_header_size, "header size" ).setDefaultValue( 0 ).setMin( 0 );
        attrSet.createTripleNumericAttribute<double>( "-pitch", pitch.x(), pitch.y(), pitch.z(), "pitch size" ).setMin ( 0.0001, 0.0001, 0.0001 ).setDefaultValue( 1,1,1 );
        attrSet.createTripleNumericAttribute<double>( "-origin", origin.x(), origin.y(), origin.z(), "origin point" ).setDefaultValue( 0, 0, 0 );
        attrSet.createNumericAttribute<int> ( "-thread", this->_num_threads, "The number of threads" ).setDefaultValue( mi::SystemInfo::getNumCores() ).setMin( 1 );
        attrSet.createNumericAttribute<T>( "-iso", this->_isovalue, "isovalue" ).setMandatory();
        attrSet.createBooleanAttribute( "-fill", this->_fillHole, "fill hole by polygons" );
        attrSet.createBooleanAttribute( "-auto", this->_auto, "automatic estimation of -hole parameter" );
        attrSet.createNumericAttribute<double>( "-hole", this->_hole, "size of hole" ).setMin( 0.0001 ).setDefaultValue( 30 );
        return ;
}

template<typename T>
ExtractEndocastCommand<T>::~ExtractEndocastCommand ( void )
{
        return;
}

template<typename T>
bool
ExtractEndocastCommand<T>::init ( const mi::Argument& arg )
{
        if ( ! this->getAttributeSet().parse( arg ) ) return false;

        if ( this->isDebugModeOn() ) {
                mi::VolumeDataUtility::setDebugModeOn();
        }
        mi::VolumeDataUtility::numThreads() =  this->_num_threads;
        this->_ctData.init( mi::VolumeInfo( this->_size, this->_pitch, this->_origin ) );
        if ( ! mi::VolumeDataUtility::open( this->_ct_file, this->_ctData, this->_header_size ) ) return false;
        return true;
}

template<typename T>
bool
ExtractEndocastCommand<T>::run  ( void )
{
	const mi::VolumeData<T>& ctData = this->_ctData;
        const mi::VolumeInfo& info = const_cast< mi::VolumeData<T>& >(ctData).getInfo();
	const std::string fileHeader = mi::FileNameConverter( this->_ct_file ).removeExtension();	
	
	mi::VolumeData<char> binaryData( info );
	if ( !mi::Routine::run(BinarizationRoutine<T>( ctData, binaryData ).setThreshold(this->_isovalue).setTempFileNameHeader(fileHeader).getInstance())) return false;
	
        mi::VolumeData<char>  labelData( info );
	if ( !mi::Routine::run(WatershedRoutine( binaryData,labelData).setTempFileNameHeader(fileHeader).getInstance() ) ) return false;
        binaryData.deallocate();

	// endocast : 2
	mi::Range range( info );
	for( auto&& p : range) {
		char v = 0;
		if ( labelData.get( p ) == 2 ) v = 1;
		labelData.set(p, v);
	}

	mi::VolumeDataPolygonizer<T> polygonizer( ctData, labelData);
	polygonizer.polygonize(this->_isovalue, this->_endocast_polygon);
	this->_endocast_polygon.stitch(1.0e-10);

/*	
	if ( mi::Routine::run( MaskingRoutine( labelData,this->_ctData).getInstance()) ) return false;
*/
        return true;
}

template<typename T>
bool
ExtractEndocastCommand<T> ::term ( void )
{
	return mi::TriMeshExporterObj(this->_endocast_polygon).write( this->_output_file);
}

template class ExtractEndocastCommand<char>;
template class ExtractEndocastCommand<unsigned char>;
template class ExtractEndocastCommand<short>;
template class ExtractEndocastCommand<unsigned short>;
template class ExtractEndocastCommand<int>;
template class ExtractEndocastCommand<unsigned int>;
template class ExtractEndocastCommand<float>;
template class ExtractEndocastCommand<double>;





