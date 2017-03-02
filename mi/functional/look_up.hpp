#ifndef MI_FUNCTIONAL_LOOK_UP_HPP
#define MI_FUNCTIONAL_LOOK_UP_HPP 1
#include <vector>
#include <functional>
namespace mi
{
        /**
         * @class look_up
         * Integer value conversion by look-up table.
         */
        template <typename T>
        class look_up : public std::unary_function<T, void>
        {
        private:
                const std::vector<T>& _table;        ///< look up table.
        public:
                look_up ( const std::vector<T>& table ) : _table( table ) {
                        return;
                }

                void operator () ( T& i ) const {
                        if ( i >= this->_table.size() ) i = this->_table[ this->_table.size() - 1];
                        else i = this->_table[i]; // the user must take care i < _table.size()
                        return;
                }
        };
}
#endif // MI_FUNCTIONAL_LOOK_UP_HPP
