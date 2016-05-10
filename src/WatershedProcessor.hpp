#ifndef MI_WATERSHED_PROCESSOR_HPP
#define MI_WATERSHED_PROCESSOR_HPP 1
#include <mi/VolumeData.hpp>
#include <mi/PriorityQueue.hpp>
#include <mi/NonCopyable.hpp>

namespace mi
{
        template <typename T>
        class WatershedProcessor : public NonCopyable
        {
        private:
                const VolumeData<float>& _weightData;
        public:
                WatershedProcessor ( const VolumeData<float>& weightData ) :  _weightData( weightData ) {
                        return;
                }
		
		/**
		 *
		 * @note  Label [0 : Background >1 : init labels ]
		 */
                bool process ( VolumeData<T>& labelData ) {
                        const VolumeInfo& info = const_cast<VolumeData<T>&>( labelData ).getInfo();
			
                        Range range( info.getMin(), info.getMax() );
                        if ( 1 ) { //@ the program will be failed when "if" is removed.
                                PriorityQueue <Point3i> pq;
				
                                for( Range::iterator iter = range.begin() ; iter != range.end() ; ++iter ) {
                                        const Point3i& p = *iter;
                                        if ( labelData.get( p ) > 0 && this->_weightData.get( p ) > 0 ) {
                                                pq.push( p, -this->_weightData.get( p ) ) ;
                                        }
                                }

                                while( !pq.empty() ) {
                                        const Point3i p = pq.getTopIndex();
                                        pq.pop();
                                        const int labelId = labelData.get( p ); // Label ID to be propagated.
                                        for( Neighbor::iterator diter = Neighbor::begin() ; diter != Neighbor::end( 6 ) ; ++diter ) {
                                                const Point3i np = *diter + p;
                                                if ( ! info.isValid( np ) ) continue;
                                                if ( labelData.get( np ) != 0 ) continue;
                                                if( this->_weightData.get( np ) > 0 ) {
                                                        labelData.set( np, labelId );
                                                        pq.push( np, -this->_weightData.get( np ) );
                                                }
                                        }
                                }
                        }
                        return true;
                }
        };
}

#endif// MI_WATERSHED_PROCESSOR_HPP
