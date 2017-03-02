/**
 * @file Timer.hpp
 * @author Takashi Michikawa <michikawa@acm.org>
 */
#ifndef MI_TIMER_HPP
#define MI_TIMER_HPP 1

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#else
#include <sys/time.h>
#endif

///Unit of clocks (s) .
#ifdef  __APPLE__
#define SUBSECONDS 1.0e-6
#else // linux
#define SUBSECONDS 1.0e-3
#endif

#include <ctime>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace mi
{
        /**
         * @brief Time format.
         */
        enum time_format {
                TIME_SECOND, ///< Seconds.
                TIME_MINUTE, ///< Minutes.
                TIME_HOUR,   ///< Hours.
                TIME_DAY,    ///< Days.
                TIME_AUTO    ///< Default.
        };
        /**
         * @class Timer Timer.hpp <mi/Timer.hpp>
         * @brief Timer object.
         *
         * This class can be used for measuring time.  The precision
         * is much better than the use of clock (), since we wrap more
         * accurate functions.  When you measure computation time of a
         * step, insert Timer::start("key") and Timer::end("key")
         * before and after the part.
         *
         * Here shows an example code :
         * @code
         *  mi::Timer timer;
         *  timer.start("hoge");
         *  hoge();
         *  timer.end("hoge");
         *  timer.print("hoge");
         * @endcode
         *
         */
        class Timer
        {
        private:
                Timer ( const Timer& that ) ;
                void operator = ( const Timer& that ) ;
        public:
                /**
                 * @brief Constructor.
                 */
                explicit Timer ( void ) {
                        this->init();
                        return;
                }
                /**
                 * @brief Destructor.
                 */
                ~Timer ( void ) {
                        return;
                }

                /**
                 * @brief Initialize Timer object.
                 */
                void init ( void ) {
                        this->_startTime.clear();
                        this->_endTime.clear();
                        this->_time.clear();
                        return;
                }

                /**
                 * @brief Start measurement.
                 * @param [in] key Key string for the measurement.
                 * @retval true Success.
                 * @retval false Failure. The key already exists.
                 */
                bool start ( const std::string& key ) {
                        if ( this->_startTime.find( key ) != this->_startTime.end() ) {
                                std::cerr<<"key "<<key<<" already exists."<<std::endl;
                                return false;
                        }
                        this->_startTime[ key ] = this->get_elapsed_time();
                        this->_keys.push_back( key );
                        return true;
                }
                /**
                 * @brief End measurement.
                 * @param [in] key Key string for the measurement.
                 * @retval true Success.
                 * @retval false Failure. The key does not exist.
                 */
                bool end ( const std::string& key ) {
                        if ( this->_startTime.find( key ) == this->_startTime.end() ) {
                                std::cerr<<" key "<<key<<" does not exist."<<std::endl;
                                return false;
                        }
                        this->_endTime[ key ] = this->get_elapsed_time();
                        this->_time[key] =  this->_endTime[key] - this->_startTime[key];
                        return true;
                };
                /**
                 * @brief Get the time associated with the key
                 * @param [in] key Key string for the measurement.
                 * @param [in] format Time format.
                 * @return Computation time.
                 * @retval false Failure. The measurement is not finished or does not exist.
                 */
                double get ( const std::string& key , const time_format format = TIME_SECOND ) {
                        if ( !this->exist( key ) ) return -1.0;

                        const double t0 =  this->_time[key];

                        if      ( format == TIME_SECOND ) return t0;
                        else if ( format == TIME_MINUTE ) return t0 / TIMER_MINUTE;
                        else if ( format == TIME_HOUR )   return t0 / TIMER_MINUTE / TIMER_HOUR;
                        else if ( format == TIME_DAY )    return t0 / TIMER_MINUTE / TIMER_HOUR / TIMER_DAY;
                        else return -1.0;// error
                }

                /**
                 * @brief Print measurement.
                 * @param [in] key Key. Print nothing when key does not match.
                 * @param [in] digit Digit.
                 * @param [in] format Time format.
                 */
                void print ( const std::string &key, const int digit = 4, const time_format format = TIME_AUTO, std::ostream& out = std::cout ) {
                        if ( !this->exist( key ) ) return;
                        out<<this->toString ( key, digit, format ) <<std::endl;
                        return;
                }
                /**
                 * @brief Print all measurements.
                 * @param [in] digit Digit.
                 * @param [in] format Time format.
                 */
                void printAll ( const int digit = 4, const time_format format = TIME_AUTO, std::ostream& out = std::cout ) {
                        for( std::vector<std::string>::iterator iter = this->_keys.begin() ; iter != this->_keys.end() ; ++iter ) {
                                this->print( *iter, digit, format, out );
                        }
                }
                /**
                 * @brief Get measurement result as string format.
                 * @param [in] key Key. Print nothing when key does not match.
                 * @param [in] digit Digit.
                 * @param [in] format Time format.
                 */

                std::string toString ( const std::string &key, const int digit = 6, const time_format format = TIME_AUTO ) {
                        if ( !this->exist( key ) ) {
                                return std::string( "NULL" );
                        }
                        if( format == TIME_AUTO ) {
                                const double t = this->get( key, TIME_SECOND );
                                if ( t < TIMER_MINUTE ) {
                                        return this->toString( key, digit, TIME_SECOND );
                                } else if ( t < TIMER_MINUTE * TIMER_HOUR ) {
                                        return this->toString( key, digit, TIME_MINUTE );
                                } else if ( t < TIMER_MINUTE * TIMER_HOUR * TIMER_DAY ) {
                                        return this->toString( key, digit, TIME_HOUR );
                                } else {
                                        return this->toString( key, digit, TIME_DAY );
                                }
                        } else {
                                std::stringstream ss;
                                ss<<key<<" : \t"<<std::setprecision ( digit )<<this->get( key , format )<<"\t";
                                if      ( format == TIME_SECOND ) ss<<"[s]";
                                else if ( format == TIME_MINUTE ) ss<<"[m]";
                                else if ( format == TIME_HOUR )   ss<<"[h]";
                                else if ( format == TIME_DAY )    ss<<"[d]";
                                else return std::string();
                                return ss.str();
                        }
                }

        private:
                /**
                 * @brief Check the key exists.
                 * @retval true The key exists.
                 * @retval false The key does not exist.
                 */
                bool exist ( const std::string& key ) const {
                        if ( this->_time.find( key ) == this->_time.end() ) {
                                std::cerr<<" the key "<<key<<" does not exist."<<std::endl;
                                return false;
                        }
                        return true;
                }
                /**
                 * @brief Get elapsed time.
                 * @return Elapsed time.
                 */
                inline double get_elapsed_time ( void ) const {
#ifdef WIN32
                        LARGE_INTEGER start_pc, freq_pc;
                        QueryPerformanceFrequency( &freq_pc );
                        QueryPerformanceCounter( &start_pc );
                        return static_cast<double>( start_pc.QuadPart ) / static_cast<double>( freq_pc.QuadPart );
#else //linux, mac
                        struct timeval tv0;
                        gettimeofday( &tv0 , NULL );
                        return tv0.tv_sec + tv0.tv_usec * SUBSECONDS;
#endif
                }
        private:
                const static int TIMER_MINUTE = 60;//seconds.
                const static int TIMER_HOUR   = 60;//minutes.
                const static int TIMER_DAY    = 24;//hours

                std::map<std::string, double> _startTime;/// Start time.
                std::map<std::string, double> _endTime; ///< End time.
                std::map<std::string, double> _time; ///< Measured time
                std::vector<std::string>      _keys; ///< Key
        };
};
#endif //MI_TIME_HPP
