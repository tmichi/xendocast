/**
 * @file VolumeDataUtility.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_VOLUME_DATA_UTILITY_HPP
#define MI_VOLUME_DATA_UTILITY_HPP 1
#include <iostream>
#include <string>

#include "ConnectedComponentLabeller.hpp"
#include "ConnectedComponentLabellerRle.hpp"
#include "DistanceFieldComputer.hpp"
#include "VolumeData.hpp"
#include "VolumeDataImporter.hpp"
#include "VolumeDataExporter.hpp"
#include "VolumeDataPolygonizer.hpp"
#include "VolumeDataUpSampler.hpp"
#include "VolumeDataClipper.hpp"
#include "ParallelFor.hpp"
#include "FunctionObject.hpp"
#include "Mesh.hpp"
namespace mi
{
        /**
         * @class VolumeUtility VolumeUtility.hpp <mi/VolumeUtility.hpp>
         * @brief Utility methods.
         */
        class VolumeDataUtility
        {
        private:
                explicit VolumeDataUtility();
                VolumeDataUtility ( const VolumeDataUtility& that );
                void operator = ( const VolumeDataUtility& that );
        public:
                /**
                 * @brief Check debug mode.
                 * @retval true Debug mode.
                 * @retval false Release mode.
                 */
                static bool& isDebugMode ( void ) {
                        static bool isDebugMode = false;
                        return isDebugMode;
                }
                /**
                 * @brief Set debug mode on.
                 */
                static void setDebugModeOn( void ) {
                        VolumeDataUtility::isDebugMode() = true;
                        return;
                }

                /**
                 * @brief Get the number of threads.
                 * @return The number of threads.
                 */
                static int& getNumThread ( void ) {
                        static int nthread = 1;
                        return nthread;
                }

                /**
                 * @brief Set the number of threads.
                 * @param [in] nThread The number of threads.
                 */
                static void setNumThread ( const int nThread ) {
                        VolumeDataUtility::getNumThread() = nThread <  1 ? 1 : nThread;
                        std::cerr<<"#threads = "<< VolumeDataUtility::getNumThread()<<std::endl;
                        return;
                }

                /**
                 * @brief Create clipped volume data.
                 * @param [in] inData Input data.
                 * @param [in] bmin Bounding box (min).
                 * @param [in] bmax Bounding box (max).
                 * @param [out] clippedData Clipped volume data.
                 * @return true Success.
                 * @return false Failure.
                 */
                template<typename T>
                static bool clip ( VolumeData<T>& inData, const Point3i& bmin, const Point3i& bmax, VolumeData<T>& clippedData ) {
                        VolumeDataClipper<T>( inData ).clip( bmin, bmax, clippedData, VolumeDataUtility::getNumThread() );
                        return true;
                }

                /**
                 * @brief Open volume data.
                 * @param [in] filename File name.
                 * @param [in] header_size Header size (byte).
                 * @param [out] data Volume data.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                template< typename T>
                static bool open ( VolumeData<T>& data,  const std::string& filename, const int header_size = 0 ) {
                        return VolumeDataImporter<T>( data, header_size ).read( filename ) ;
                }

                /**
                 * @brief Save volume data.
                 * @param [in] filename File name.
                 * @param [in] data Volume data.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                template< typename T>
                static bool save ( VolumeData<T>& data, const std::string& filename ) {
                        return VolumeDataExporter<T>( data ).write( filename ) ;
                }

                /**
                 * @brief Polygonize volume data.
                 * @param [in] data Volume data.
                 * @param [in] threshold Iso value.
                 * @param [out] mesh Mesh.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                template< typename T>
                static bool polygonize( VolumeData<T>& data, const double threshold, Mesh& mesh ) {
                        VolumeDataPolygonizer<T> polygonizer( data );
                        const int numTri = polygonizer.polygonize( threshold, mesh );
                        std::cerr<<numTri<<" triangles extracted."<<std::endl;
                        return true;
                }

                /**
                 * @brief Binarize volume data.
                 * @param [in] inData Input data.
                 * @param [out] outData Output data.
                 * @param [in] isovalue Isovalue.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                template<typename T>
                static	bool binarize( VolumeData<T>& inData, VolumeData<char>& outData, const T isovalue , const bool negate = false ) {
                        VolumeInfo& info = inData.getInfo();
                        outData.init( info );

                        const int grain_size = VolumeDataUtility::getGrainSize( info.getSize() );
                        parallel_for_each( info.begin(), info.end(), binarize_voxel<T>( inData, outData, isovalue , negate ), grain_size );

                        return true;
                }

                /**
                 * @brief Erode volume data.
                 * @param [in] inData Input data.
                 * @param [out] outData Output data.
                 * @param [in] r Size of structural element (or radius of sphere).
                 * @retval true Success.
                 * @retval false Failure.
                 */
                static	bool erode( VolumeData<char>& inData, VolumeData<char>& outData, const double r ) {
                        VolumeInfo& info = inData.getInfo();
                        const Point3i& size = info.getSize();
                        const int grain_size = size.x() * size.y() / VolumeDataUtility::getNumThread() * size.z();
                        outData.init( info );
                        mi::parallel_for_each( info.begin(), info.end(), mi::erode( inData, outData, r ), grain_size );
                        return true;
                }


                /**
                 * @brief Dilate volume data.
                 * @param [in] inData Input data.
                 * @param [out] outData Output data.
                 * @param [in] r Size of structural element (or radius of sphere).
                 * @retval true Success.
                 * @retval false Failure.
                 */
                static	bool dilate( VolumeData<char>& inData, VolumeData<char>& outData, const double r ) {

                        VolumeInfo& info = inData.getInfo();
                        const int grain_size =  VolumeDataUtility::getGrainSize( info.getSize() );
                        if ( !outData.isReadable() ) outData.init( info );
                        parallel_for_each( info.begin(), info.end(), mi::dilate( inData, outData, r ), grain_size );
                        return true;
                }

                static	bool diff( const VolumeData<char>& srcData, const VolumeData<char>& trgData, VolumeData<char>& outData ) {
                        VolumeInfo& info = const_cast<VolumeData<char>&>( srcData ).getInfo();
                        if ( !outData.isReadable() ) outData.init( info );

                        const int grain_size =  VolumeDataUtility::getGrainSize( info.getSize() );
                        parallel_for_each( info.begin(), info.end(), mi::diff( srcData, trgData, outData ), grain_size );
                        return true;
                }

                template< typename T>
                static bool negate_binary ( const VolumeData<T>& inData, VolumeData<T>& outData ) {
                        VolumeInfo& info = const_cast<VolumeData<T>&>( inData ).getInfo();
                        const int grain_size =  VolumeDataUtility::getGrainSize( info.getSize() );
                        if ( !outData.isReadable() ) outData.init( info );
                        parallel_for_each( info.begin(), info.end(), mi::negate_binary<T>( inData, outData ), grain_size );
                        return true;
                }


                /**
                 * @brief Compute offset volume data.
                 * @param [in] inData Input data.
                 * @param [out] outData Output data.
                 * @radius [in] Radius of sphere.
                 * @retval true Success.
                 * @retval false Failure.
                */
                static	bool offset ( VolumeData<char>& inData, VolumeData<char>& outData, double radius ) {
                        const int grain_size = 1000000;
                        VolumeInfo& info = inData.getInfo();
                        outData.init( info );
                        parallel_for_each( info.begin(), info.end(), mi::dilate( inData, outData, radius ), grain_size );
                        return true;
                }

                /**
                 * @brief Extract boundary voxels
                 * @param [in] inData Input data.
                 * @param [out] boundaryData Output data.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                static	bool extractBoundaryVoxels ( VolumeData<char>& inData, VolumeData<char>& boundaryData ) {
                        const int grain_size = 1000000;
                        VolumeInfo& info = inData.getInfo();
                        boundaryData.init( info );
                        parallel_for_each( info.begin(), info.end(), extract_boundary( inData, boundaryData ), grain_size );
                        return true;
                }
                /**
                 * @brief set 0 to boundary voxels
                 * @param [in, out] data Volume data.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                static	bool clearBoundaryVoxel ( VolumeData<char>& data ) {
                        const VolumeInfo& info = data.getInfo();
                        const Point3i& size = info.getSize();
                        for ( int z = 0 ; z < size.z() ; ++z ) {
                                for ( int y = 0 ; y < size.y() ; ++y ) {
                                        for ( int x = 0 ; x < size.x() ; ++x ) {
                                                char value = data.get( x,y,z );
                                                if ( x == 0            ) value = 0;
                                                if ( x == size.x() - 1 ) value = 0;
                                                if ( y == 0            ) value = 0;
                                                if ( y == size.y() - 1 ) value = 0;
                                                if ( z == 0            ) value = 0;
                                                if ( z == size.z() - 1 ) value = 0;
                                                data.set( x,y,z, value );
                                        }
                                }
                        }
                        return true;
                }

                /**
                 * @brief Assign labels to each components.
                 * @param [in] binaryImage Binary volume data.
                 * @param [out] labelImage The result.
                 * @poaram [in] neighbor using connectivity (6, 18, 26)
                 *
                 */
                static bool label( VolumeData<char>& binaryImage, VolumeData<int>& labelImage, const int neighbor = 26, const bool isSorted = true, int* numLabels=NULL ) {
                        if ( !labelImage.init( binaryImage.getInfo(), true ).check() ) return false; //not allocated.
                        ConnectedComponentLabeller labeller( binaryImage );
                        const int nLabel = labeller.label( labelImage, neighbor, isSorted ) ; // 0: background, 1 : largest component.
                        if ( numLabels != NULL ) *numLabels = nLabel;
                        return true;
                }
                template< typename T>
                static bool label_rle( VolumeData<char>& binaryImage, VolumeData<T>& resultImage,  const int neighbor = 26, const bool isSorted = true, int* numLabels=NULL ) {
                        mi::ConnectedComponentLabellerRle labeller( binaryImage );
                        *numLabels =  labeller.label( isSorted );
                        // size check
                        labeller.getData( resultImage );
                        return true;
                }

                static bool extract_nth_component(  VolumeData<char>& binaryImage, VolumeData<char>& resultImage, const int n = 1 ) {
                        mi::ConnectedComponentLabellerRle labeller( binaryImage );
                        const int num = labeller.label( true );
                        if ( n > num ) return false;
                        labeller.getNthComponent( resultImage, n );
                        return true;
                }


                static bool extract_not_nth_components(  VolumeData<char>& binaryImage, VolumeData<char>& resultImage, const int n = 1 ) {
                        mi::ConnectedComponentLabellerRle labeller( binaryImage );
                        const int num = labeller.label( true );
                        if ( n > num ) return false;
                        labeller.getNotNthComponents( resultImage, n );
                        return true;
                }


                /**
                 * @brief Shuffle label ID
                 * @param [in, out] labelImage Labels.
                 * @param [in] numLabel The number of labels.
                 */
                static void shuffleLabels ( VolumeData<int>& labelImage, const int numLabels ) {
                        // 0 : background (unchanged)
                        // 1 - n : foreground
                        std::vector<int> id;
                        for( int i = 0 ; i <= numLabels ; ++i ) id.push_back( i );
                        std::random_shuffle( id.begin()+1 , id.end() );
                        std::for_each ( labelImage.begin(), labelImage.end() , look_up<int>( id ) ) ;
                        return;
                }

                /**
                 * @brief Compute distance fields.
                 * @param [in] inData Input data.
                 * @param [out] outData Output data.
                 * @retval true Success.
                 * @retval false Failure.
                 */

                static bool compute_distance_field ( VolumeData<char>& inData, VolumeData<Vector3s>& outData ) {
                        VolumeInfo& info = inData.getInfo();
                        outData.init( info );

                        DistanceFieldComputer computer ( inData, outData );
                        if ( !computer.compute( VolumeDataUtility::getNumThread() ) ) return false;
                        return true;
                }
                static bool compute_distance_field ( VolumeData<char>& inData, VolumeData<float>& outData ) {
                        VolumeData<Vector3s> vdf ( inData.getInfo() );
                        if ( !VolumeDataUtility::compute_distance_field( inData, vdf ) ) return false;
                        if ( !VolumeDataUtility::vdf2df( vdf, outData ) ) return false;
                        return true;
                }
                /**
                 * @brief Convert Vector distance fields to scalar distance field.
                 * @param [in] vdf Vector distance field.
                 * @param [in] df Scalar distance field.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                static bool vdf2df( VolumeData<Vector3s>& vdf, VolumeData<float>& sf ) {
                        const Point3i& size = vdf.getInfo().getSize();
                        const int grain_size = size.x() * size.y() / VolumeDataUtility::getNumThread() * size.z();
                        sf.init( vdf.getInfo() );
                        parallel_for_each ( sf.getInfo().begin(), sf.getInfo().end(), vec2dist( vdf, sf ), grain_size );
                        return true;
                }

                /**
                 * @brief Upsample volume data.
                 * @param [in] inData Input data.
                 * @param [out] outData Onput data.
                 * @param [in] scale Scaling factor.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                template <typename T>
                static bool upsample ( VolumeData<T>& inData, VolumeData<T>& outData, const int scale = 1 ) {
                        if ( !VolumeDataUpSampler<T> ( inData, outData ).sample( scale ) ) {
                                return false;
                        }

                        Point3i size  = outData.getInfo().getSize();
                        Point3d pitch = outData.getInfo().getPitch();
                        std::cerr<<"new size : "<<size.x()<<" "<<size.y()<<" "<<size.z()<<std::endl;
                        std::cerr<<"new pitch : "<<pitch.x()<<" "<<pitch.y()<<" "<<pitch.z()<<std::endl;
                        return true;
                }
                /**
                 * @brief Polygonize volume data.
                 * @param [in] inData Input volume data.
                 * @param [out] mesh Mesh data.
                 * @param [in] isovalue Isovalue.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                template<typename T>
                static bool polygonize( VolumeData<T>& inData, Mesh& mesh, const double isovalue = 0 ) {
                        std::cerr<<"now creating polygons"<<"... ";
                        VolumeDataPolygonizer<T> polygonizer( inData );
                        const int numTri = polygonizer.polygonize( static_cast<float>( isovalue ), mesh );
                        std::cerr<<"done."<<std::endl;
                        std::cerr<<numTri<<" triangles extracted."<<std::endl;
                        return true;
                }

                /*
                 * may be deleted.
                 */
                template< typename T>
                static bool debug_save ( VolumeData<T>& data, const std::string& filename ) {
                        if ( !VolumeDataUtility::isDebugMode() ) return true; // do nothing.
                        if ( !VolumeDataUtility::save<T>( data, filename ) ) return false;
                        std::cerr<<"[debug] the result was saved to "<<filename<<std::endl;
                        return true;
                }

        private:
                static int getGrainSize( const mi::Point3i& size ) {
                        return static_cast<int>( size.x() * size.y() / VolumeDataUtility::getNumThread() * size.z() );

                }
        };
}
#endif // MI_VOLUME_DATA_UTILITY_HPP




