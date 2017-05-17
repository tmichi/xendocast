#include "ExtractEndocastCommand.hpp"
#include <mi/MeshUtility.hpp>
#include <mi/VolumeDataUtility.hpp>
#include <mi/VolumeDataPolygonizer.hpp>
#include <mi/Neighbor.hpp>
#include <mi/FileNameConverter.hpp>
//#include <mi/SystemInfo.hpp>
#include <mi/WatershedProcessor.hpp>
#include "Binarizer.hpp"
#include <mi/VolumeDataCreator.hpp>

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
	// Commented
	attrSet.createNumericAttribute<int> ( "-thread", this->_num_threads, "The number of threads" ).setDefaultValue( 4 ).setMin( 1 );
	//attrSet.createNumericAttribute<int> ( "-thread", this->_num_threads, "The number of threads" ).setDefaultValue( mi::SystemInfo::getNumCores() ).setMin( 1 );
        
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
	//Commented 
	//mi::SystemInfo::print( mi::Logger::getStream() );
	this->getAttributeSet().print( mi::Logger::getStream() );

	// initialization of the volume.
        mi::VolumeDataUtility::setNumThread( this->_num_threads );
        this->_ctData.init( mi::VolumeInfo( this->_size, this->_pitch, this->_origin ) );
        if ( ! mi::VolumeDataUtility::open( this->_ctData, this->_ct_file, this->_header_size ) ) return false;
        return true;
}

template<typename T>
bool
ExtractEndocastCommand<T>::run  ( void )
{
	std::cerr<<"run"<<std::endl;
        const mi::VolumeInfo& info = const_cast<mi::VolumeData<T>&>( this->_ctData ).getInfo();
	
        mi::VolumeData<char> binaryData( info );
        this->getTimer().start("initialize");
        this->binarize( this->_ctData, binaryData ) ;
	std::cerr<<"binarize"<<std::endl;
        mi::VolumeData<float> distData( info );
	std::cerr<<"df"<<std::endl;
        if( !mi::VolumeDataUtility::compute_distance_field( binaryData, distData ) ) return false; // binary -> vdf
        mi::VolumeDataUtility::debug_save( distData, this->create_file_name( "dist", "raw" ) );
        binaryData.deallocate();
        this->getTimer().end("initialize");
	this->getTimer().start( "watershed" );
	std::cerr<<"ws"<<std::endl;
        mi::VolumeData<char>  labelData( info );
        this->watershed( distData, labelData );
        distData.deallocate();
        this->getTimer().end( "watershed" );
        this->getTimer().start( "polygonize" );
        this->polygonize_endocast( labelData );
        this->getTimer().end( "polygonize" );
        return true;
}

template<typename T>
bool
ExtractEndocastCommand<T> ::term ( void )
{
        return mi::MeshUtility::save( this->_endocast_polygon, this->_output_file );
}

template<typename T>
bool
ExtractEndocastCommand<T> ::binarize ( const mi::VolumeData<T>& ctData, mi::VolumeData<char>& binaryData )
{
        const mi::VolumeInfo& info = const_cast<mi::VolumeData<T>&>( this->_ctData ).getInfo();
        mi::VolumeData<char> tmpData( info );
        Binarizer<T, char> binarizer( this->_ctData, 1, 0 );
        binarizer.binarize( this->_isovalue, tmpData );

        // remove smaller components.
        mi::VolumeDataUtility::extract_nth_component( tmpData, binaryData, 1 );

        mi::Range range( info.getMin(), info.getMax() );
        for( mi::Range::iterator iter = range.begin() ; iter != range.end() ; ++iter ) {
                const mi::Point3i& p = *iter;
                const char value = 1 - binaryData.get( p );
                binaryData.set( p, value );
        }
        mi::VolumeDataUtility::debug_save( binaryData, this->create_file_name( "binary", "raw" ) );
        return true;
}


template<typename T>
bool
ExtractEndocastCommand<T>::watershed( mi::VolumeData<float>& distData, mi::VolumeData<char>& labelData )
{
	//@note あまりにも大きいのははじいたほうがよい。
        const mi::VolumeInfo& info = const_cast<mi::VolumeData<T>&>( this->_ctData ).getInfo();
        mi::Range range( info.getMin(), info.getMax() );

        if ( this->_auto ) {
                std::cerr<<"automatic mode."<<std::endl;
                mi::VolumeDataCreator<char> creator( labelData ) ;
                creator.setValue( 1 );
                for( mi::Range::iterator iter = range.begin() ; iter != range.end() ; ++iter ) {
                        const mi::Point3i& p = *iter;
                        if ( info.isCorner( p ) && labelData.get( p ) == 0 ) {
                                const float dist = distData.get( p );
                                if ( dist <= 0 ) continue;
                                creator.fillSphere( p, dist * 0.9f );
                        }
                }

                mi::Point3i maxp;
                float maxDist = 0;
                for( mi::Range::iterator iter = range.begin() ; iter != range.end() ; ++iter ) {
                        const mi::Point3i p = *iter;
                        if ( labelData.get( p ) == 1 ) continue;
                        const float dist = distData.get( p );
                        if ( maxDist < dist   ) {
                                maxDist = dist;
                                maxp = p;
                        }

                }
                creator.setValue( 2 );
                creator.fillSphere( maxp, maxDist * 0.5f );
        } else {
                mi::VolumeData<char> initData( info );
                float maxDist = this->_hole;
                for( mi::Range::iterator iter = range.begin() ; iter != range.end() ; ++iter ) {
                        const mi::Point3i p = *iter;
                        const char value = ( distData.get( p ) < maxDist ) ? 0 : 1;
                        initData.set( p,value );
                }
                
                
                // 0 : fg, 1 : bg 2 : ec. 3~ : others.
                int numLabel = 0;
                if( !mi::VolumeDataUtility::label_rle( initData, labelData, 26, true, &numLabel ) ) return false;
        }

        mi::VolumeDataUtility::debug_save( labelData, this->create_file_name( "init_label", "raw" ) );

	// @note 141204
	// Added for saving label results.
/*	if ( 0 ) {
		mi::VolumeData<char> labelBinary ( info );
                for( mi::Range::iterator iter = info.begin() ; iter != info.end() ; ++iter ) {
                        const mi::Point3i p = *iter;
                        const char value = ( labelData.get(p) == 2 ) ? 1 : 0;
                        labelData.set( p,value );
                }
		mi::VolumeDataUtility::debug_save( labelData, this->create_file_name( "labelg", "raw" ) );
	}
*/
      mi::WatershedProcessor<char> processor( distData );
        processor.process( labelData );
        std::cerr<<"watershed computed."<<std::endl;
        mi::VolumeDataUtility::debug_save( labelData, this->create_file_name( "label", "raw" ) );
        return true;
}


template<typename T>
bool
ExtractEndocastCommand<T>::polygonize_endocast( mi::VolumeData<char>& labelData )
{

        const mi::VolumeInfo& info = const_cast<mi::VolumeData<T>&>( this->_ctData ).getInfo();
        mi::Range range( info.getMin(), info.getMax() );
        mi::Mesh mesh;
        if ( this->_fillHole ) {
                for( mi::Range::iterator iter = range.begin() ; iter != range.end() ; ++iter ) {
                        const mi::Point3i p = *iter;
                        const int label = labelData.get( p );
                        if ( label != 0 && label != 2 ) {
                                this->_ctData.set( p, this->_isovalue ) ;
                        }
                }
                mi::VolumeDataUtility::debug_save( this->_ctData, this->create_file_name( "filled", "raw" ) );
                mi::VolumeDataUtility::polygonize( this->_ctData, mesh, this->_isovalue );
        } else {
                mi::VolumeDataPolygonizer<T> polygonizer( this->_ctData );
                mi::VolumeData<char> mask ( info ) ;
                for( mi::Range::iterator iter = range.begin() ; iter != range.end() ; ++iter ) {
                        const mi::Point3i p = *iter;
                        if ( labelData.get( p ) == 2 ) { // check
                                mask.set( p, 1 ) ; //check
                        } else mask.set( p, 0 );
                }
                polygonizer.polygonize( static_cast<float>( this->_isovalue ),  mesh, mask );
        }

        mi::MeshUtility::stitch( mesh );
        this->_endocast_polygon.clone( mesh );
        mesh.negateOrientation();
        mi::AssemblyMesh assy;
        mi::MeshUtility::decompose( mesh, assy );
        int max_num_faces = 0;
        int id = 0;
        for ( int i = 0 ; i < assy.getNumMeshes() ; ++i ) {
                const int num_faces =assy.getMesh( i )->getNumFaces();
                if ( max_num_faces < num_faces ) {
                        id = i ;
                        max_num_faces = num_faces;
                }
        }
        this->_endocast_polygon.clone( *assy.getMesh( id ) );
        mi::MeshUtility::stitch( this->_endocast_polygon );
	mi::Logger::getStream()<<"#triangles : "<<this->_endocast_polygon.getNumFaces()<<std::endl;
        return true;
}

template<typename T>
std::string
ExtractEndocastCommand<T>::create_file_name ( const std::string& tail, const std::string& ext )
{
        mi::FileNameConverter converter( this->_ct_file );
        std::stringstream ss;
        ss<<converter.removeExtension()<<"-"<<tail<<"."<<ext;
        return ss.str();
}

template class ExtractEndocastCommand<char>;
template class ExtractEndocastCommand<unsigned char>;
template class ExtractEndocastCommand<short>;
template class ExtractEndocastCommand<unsigned short>;
template class ExtractEndocastCommand<int>;
template class ExtractEndocastCommand<unsigned int>;
template class ExtractEndocastCommand<float>;
template class ExtractEndocastCommand<double>;





