#ifndef MI_RUN_LENGTH_CODE_BINARY_HPP
#define MI_RUN_LENGTH_CODE_BINARY_HPP 1
namespace mi
{
        class RunLengthCodeBinary
        {
        private:
                // Start point
                short int _sx;
                short int _sy;
                short int _sz;
		
                short int _length;
               
		// Parent ID (for labelling)
                int       _parent;
        public:
                RunLengthCodeBinary ( const short int x, const short int y, const short int z, const short int l ) : _sx( x ),  _sy( y ), _sz( z ) , _length( l ) {
                        this->_parent = -1;
                        return;
                }

                RunLengthCodeBinary ( const RunLengthCodeBinary& that ) : _sx( that._sx ), _sy( that._sy ), _sz( that._sz ), _length ( that._length ), _parent( that._parent ) {
                        return ;
                }

                ~RunLengthCodeBinary ( void ) {
                        return;
                }

                void setParent( const int parent ) {
                        this->_parent  = parent;
                }

                int getParent( void ) {
                        return this->_parent;
                }

                bool isConnected ( const RunLengthCodeBinary &that ) {
                        return this->is_connected_26( that );
                }

                int getLength( void ) const {
                        return this->_length;
                }

                void get( short int &sx, short int &sy, short int &sz, short int& l ) {
                        sx = this->_sx;
                        sy = this->_sy;
                        sz = this->_sz;
                        l = this->_length;
                }
        private:
                bool is_connected_26 ( const RunLengthCodeBinary& that ) {
                        const int dy = std::abs( this->_sy - that._sy );
                        const int dz = std::abs( this->_sz - that._sz );
                        if ( dy > 1 || dz > 1 ) return false;

                        const short int mn0 = this->_sx;
                        const short int mx0 = this->_sx + this->_length - 1;

                        const short int mn1 = that._sx;
                        const short int mx1 = that._sx + that._length - 1;

                        if ( mn0 - 1 <= mn1 && mn1 <= mx0 + 1 ) return true;
                        if ( mn0 - 1 <= mx1 && mx1 <= mx0 + 1 ) return true;
                        if ( mn1 - 1 <= mn0 && mn0 <= mx1 + 1 ) return true;
                        if ( mn1 - 1 <= mx0 && mx0 <= mx1 + 1 ) return true;
                        return false;
                }
        };
}
#endif //
