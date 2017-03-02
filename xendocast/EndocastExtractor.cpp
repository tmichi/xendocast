#include "EndocastExtractor.hpp"
#include <mi/VolumeDataUtility.hpp>
#include <mi/PriorityQueue.hpp>


EndocastExtractor::EndocastExtractor ( void )
{

}
EndocastExtractor::~EndocastExtractor ( void )
{
        std::cerr<<"bye"<<std::endl;
}
bool
EndocastExtractor::extract ( mi::VolumeData<float>& distData, mi::VolumeData<int>& labelData, const double scale )
{
        if ( !this->init( distData, labelData, scale ) ) return false;
        if ( !this->run( distData, labelData ) ) return false;
        std::cerr<<"extract done"<<std::endl;
        return true;
}


bool
EndocastExtractor::init ( mi::VolumeData<float>& distData, mi::VolumeData<int>& labelData, const double scale )
{
        const mi::Point3i& size = distData.getInfo().getSize();

        mi::VolumeData<char> initData( distData.getInfo() );
        labelData.init( distData.getInfo() );
        float maxDist = 0;

        // ÇÕÇµÇ¡Ç±Ç…ÇÕÅCñ≥èåèÇ≈ÉâÉxÉãÇÇ¬ÇØÇÈ

        for ( int z = 0 ; z < size.z() ; ++z ) {
                for ( int y = 0 ; y < size.y() ; ++y ) {
                        for ( int x = 0 ; x < size.x() ; ++x ) {
                                float dist = distData.get( x,y,z );
                                if ( dist <= 0 ) continue; //  background
                                // push to initData when the voxel is boundary.
                                if (    x == 0 || y == 0 || z == 0 ||
                                        x == size.x() -1 || y == size.y() - 1 || z == size.z() - 1 ) {
                                        initData.set( x,y,z, 1 );
                                        continue;
                                }
                                if ( maxDist < dist ) maxDist = dist;
                        }
                }
        }
        maxDist *= static_cast<float>( scale );

        std::cerr<<"ok"<<std::endl;
        for ( int z = 0 ; z < size.z() ; ++z ) {
                for ( int y = 0 ; y < size.y() ; ++y ) {
                        for ( int x = 0 ; x < size.x() ; ++x ) {
                                mi::Point3i v( x,y,z );
                                if ( distData.get( v ) >= maxDist ) {
                                        initData.set( v,1 );
                                }
                        }
                }
        }
        int numLabel = 0;
        // 0 : fg, 1 : bg 2 : ec. 3~ : others.
        if( !mi::VolumeDataUtility::label( initData, labelData, 26, true, &numLabel ) ) return false;
        return true;
}
bool
EndocastExtractor::run ( mi::VolumeData<float>& distData, mi::VolumeData<int>& labelData )
{
        mi::VolumeInfo& info = distData.getInfo();
        const mi::Point3i& size = distData.getInfo().getSize();
        mi::PriorityQueue <mi::Point3i> pq;

        for ( int z = 0 ; z < size.z() ; ++z ) {
                for ( int y = 0 ; y < size.y() ; ++y ) {
                        for ( int x = 0 ; x < size.x() ; ++x ) {
                                mi::Point3i p ( x,y,z );
                                if ( labelData.get( p ) > 0 && distData.get( p ) > 0 ) pq.push( p, -distData.get( p ) ) ;
                        }
                }
        }
        while( ! pq.empty() ) {
                const mi::Point3i p = pq.getTopIndex();
                pq.pop();

                const int labelId = labelData.get( p ); // Label ID to be propagated.
                for( int dz = -1 ; dz <= 1 ; ++dz ) {
                        for( int dy = -1 ; dy <= 1 ; ++dy ) {
                                for( int dx = -1 ; dx <= 1 ; ++dx ) {
                                        const mi::Point3i dp( dx, dy, dz );
                                        const mi::Point3i np = dp + p;
                                        if( ! info.isValid( np ) ) continue;
                                        const float dist = distData.get( np );
                                        const int label = labelData.get( np );
                                        if( dist > 0 && label == 0 ) {
                                                pq.push( np, -dist );
                                                labelData.set( np, labelId );
                                        }
                                }
                        }
                }
        }
        return true;
}