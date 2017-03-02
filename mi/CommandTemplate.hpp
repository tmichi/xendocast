/**
* @file  CommandTemplate.hpp
* @author Takashi Michikawa <michikawa@acm.org>
*/
#ifndef MI_COMMAND_TEMPLATE_HPP
#define MI_COMMAND_TEMPLATE_HPP 1

#include <cstdlib>

#include "Argument.hpp"
#include "ArgumentImporter.hpp"
#include "ArgumentExporter.hpp"
#include "AttributeSet.hpp"
#include "SystemInfo.hpp"
#include "Logger.hpp"
#include "Timer.hpp"
namespace mi
{
        /**
        * @class CommandTemplate CommandTemplate.hpp "mi/CommandTemplate.hpp"
        * @brief Template for CUI-based command.
        */
        class CommandTemplate
        {
        private:
                CommandTemplate ( const CommandTemplate& that );
                void operator = ( const CommandTemplate& that );
        public:
                /**
                * @brief Constructor.
                */
                explicit CommandTemplate ( const std::string& cmdStr = "a.out" ) : _cmdStr( cmdStr ),  _isTimingModeOn ( false ), _isDebugModeOn( false ) {
                        return;
                }
                /**
                * @brief Destructor.
                */
                virtual ~CommandTemplate ( void ) {
                        if( this->isTimingModeOn() ) {
                                this->getTimer().end( "total" );
                                this->getTimer().printAll( 4, mi::TIME_SECOND, mi::Logger::getStream() );
                        }
                        mi::Logger::getStream()<<"peakMemory : "<<SystemInfo::getPeakMemorySize()<<"[mb]"<<std::endl;
                        return;
                }
                /**
                 * @brief Initialize the command.
                 * @param [in] arg Argument.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                virtual bool init ( const Argument& arg ) = 0;
                /**
                 * @brief Run the command.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                virtual bool run  ( void ) = 0;
                /**
                 * @brief Terminate the command.
                 * @retval true Success.
                 * @retval false Failure.
                 */
                virtual bool term ( void ) = 0;

        protected:
                /**
                 * @brief Get attribute set.
                 * @return A reference to the attribute set.
                 */
                inline AttributeSet& getAttributeSet( void ) {
                        return this->_attr;
                }

                /**
                 * @brief Get timer object.
                 * @return A reference to the timer.
                 */
                inline Timer& getTimer( void ) {
                        return this->_timer;
                }
                /**
                * @brief Check the timing mode
                * @retval true On.
                * @retval true Off.
                */
                inline bool isTimingModeOn ( void ) const {
                        return this->_isTimingModeOn;
                }

                inline bool isDebugModeOn( void ) const {
                        return this->_isDebugModeOn;
                }
                /**
                * @brief Set timing mode on.
                */
                CommandTemplate& setTimingModeOn ( void ) {
                        this->_isTimingModeOn = true;
                        if( this->isTimingModeOn() ) {
                                this->getTimer().init();
                                this->getTimer().start ( "total" );
                        }
                        return *this;
                }
                /**
                * @brief Print usage.
                */
                CommandTemplate& setDebugModeOn ( void ) {
                        this->_isDebugModeOn = true;
                        return *this;
                }
        private:
                /**
                * @brief Print usage.
                */
                inline void usage ( void ) {
                        this->getAttributeSet().printUsage( this->_cmdStr );
                        return;
                }
                /**
                * @brief Print version.
                */
                inline void version ( void ) {
                        std::cerr<<"warning : not implemented"<<std::endl;
                        return;
                }
        public:
                /**
                * @brief Template procedure from initialization to termination.
                * @param [in] cmd Instance.
                * @param [in] arg Argument.
                * @retval 2 Success. Version information called.
                * @retval 1 Success. Usage called.
                * @retval 0 Success. Execution succeeded.
                * @retval -1 Failure. Initialzation  failed.
                * @retval -2 Failure. Running failed.
                * @retval -3 Failure. Termination failed.
                */
                static int execute ( CommandTemplate& cmd, Argument& arg ) {
                        // read .arg file.
                        if ( arg.exist( "--arg" ) ) {
                                std::string argFile = arg.get<std::string>( "--arg" );
                                ArgumentImporter( arg ).read( argFile );
                        }

                        // Message mode
                        if ( arg.exist( "--log" ) ) {
                                Logger::init( cmd._cmdStr ) ;
                        }
                        if ( arg.exist( "--debug" ) ) {
                                cmd.setDebugModeOn();
                                std::cerr<<"debug mode on."<<std::endl;
                        }
                        if ( arg.exist( "--timing" ) ) {
                                cmd.setTimingModeOn();
                        }

                        if ( arg.exist ( "--version" ) ) {
                                cmd.version();
                                return 1;
                        }
                        if ( arg.exist ( "--help" ) ) {
                                cmd.usage();
                                return 2;
                        }

                        if ( !cmd.init ( arg ) ) {
                                std::cerr<<"initialization failed."<<std::endl;
                                cmd.usage();
                                return -1;
                        }
                        if ( !cmd.run()  ) {
                                std::cerr<<"main routine failed."<<std::endl;
                                return -2;
                        }
                        if ( !cmd.term() ) {
                                std::cerr<<"termination failed."<<std::endl;
                                return -3;
                        }
                        return 0;
                }
        private:
                std::string  _cmdStr; ///< Command name
                AttributeSet _attr;   ///< Attribute set.
                Timer        _timer;///< Timer.
                bool         _isTimingModeOn; ///< Flag for timing mode.
                bool         _isDebugModeOn;///< Flag for debug mode.
        };//class CommandTemplate
}//namespace mi
#endif // MI_COMMAND_TEMPLATE_HPP


