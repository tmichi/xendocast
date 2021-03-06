#ifndef MI_SYSTEM_INFO_HPP
#define MI_SYSTEM_INFO_HPP 1
#include <iostream>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)// Win32 API
#define OS_WINDOWS 1
#include <windows.h>
#include <process.h>
#include <intrin.h>
///@note Windows platform SDK is required to use this feature on Windows.
#ifndef PEAK_MEMORY_COUNTER_DISABLED
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")
#endif// PEAK_MEMORY_COUNTER_DISABLED
//#elif defined(__APPLE__)
#else
#include <ctime>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#endif//

namespace mi
{
        class SystemInfo
        {
        public:
                static std::string getCpuName ( void ) {
#ifdef OS_WINDOWS
                        char CPUString[0x20];
                        char CPUBrandString[0x40];
                        int CPUInfo[4] = {-1};
                        __cpuid( CPUInfo, 0 );
                        unsigned int nIds = CPUInfo[0];
                        memset( CPUString, 0, sizeof( CPUString ) );
                        *( ( int* )CPUString ) = CPUInfo[1];
                        *( ( int* )( CPUString+4 ) ) = CPUInfo[3];
                        *( ( int* )( CPUString+8 ) ) = CPUInfo[2];

                        // Calling __cpuid with 0x80000000 as the InfoType argument
                        // gets the number of valid extended IDs.
                        __cpuid( CPUInfo, 0x80000000 );
                        unsigned int nExIds = CPUInfo[0];
                        memset( CPUBrandString, 0, sizeof( CPUBrandString ) );

                        // Get the information associated with each extended ID.
                        for ( unsigned int i=0x80000000; i<=nExIds; ++i ) {
                                __cpuid( CPUInfo, i );
                                // Interpret CPU brand string and cache information.
                                if  ( i == 0x80000002 )
                                        memcpy( CPUBrandString, CPUInfo, sizeof( CPUInfo ) );
                                else if  ( i == 0x80000003 )
                                        memcpy( CPUBrandString + 16, CPUInfo, sizeof( CPUInfo ) );
                                else if  ( i == 0x80000004 )
                                        memcpy( CPUBrandString + 32, CPUInfo, sizeof( CPUInfo ) );
                        }

                        std::string name;
                        if  ( nExIds >= 0x80000004 )
                                name = std::string( CPUBrandString );
                        return name;
#else
                        return SystemInfo::get_sysctl( "machdep.cpu.brand_string" );
#endif
                }
                /**
                 * @return Memory size [gb];
                 */
                static double  getMemorySize( void ) {
#ifdef OS_WINDOWS
                        MEMORYSTATUSEX stat;
                        stat.dwLength = sizeof( MEMORYSTATUSEX );
                        if ( GlobalMemoryStatusEx( &stat ) == 0 ) {
                                std::cerr<<"error while calling GlobalMemoryStatusEx()"<<std::endl;
                                return 0;
                        }
                        /// @note The retrun value is somewhat smaller than actual size.
                        return static_cast<double>( stat.ullTotalPhys / 1024.0 / 1024 / 1024 ) ;
#else
                        return SystemInfo::get_sysctl_double( "hw.memsize" ) * 1.0 / 1024.0 / 1024.0 / 1024.0;
#endif
                }


                static int getNumCores ( void ) {
#ifdef OS_WINDOWS
                        SYSTEM_INFO sysinfo;
                        GetSystemInfo( &sysinfo );
                        return static_cast<int>( sysinfo.dwNumberOfProcessors );
#else
                        return SystemInfo::get_sysctl_int( "machdep.cpu.core_count" );
#endif
                }

                static std::string getDate( void ) {
#ifdef OS_WINDOWS
                        SYSTEMTIME systime;
                        GetLocalTime( &systime );
                        std::stringstream ss;
                        ss<<systime.wYear<<"-"<<std::setw( 2 )<<std::setfill( '0' )<<systime.wMonth<<"-"
                          <<std::setw( 2 )<<std::setfill( '0' )<<systime.wDay<<"-"
                          <<std::setw( 2 )<<std::setfill( '0' )<<systime.wHour<<":"
                          <<std::setw( 2 )<<std::setfill( '0' )<<systime.wMinute<<":"
                          <<std::setw( 2 )<<std::setfill( '0' )<<systime.wSecond;
                        return ss.str();
#else
                        time_t rawtime = std::time( NULL );
                        struct tm * timeinfo = localtime ( &rawtime );
                        char buffer [80];
                        strftime ( buffer,80,"%F-%X",timeinfo );
                        std::string result( buffer );
                        return result;
#endif
                }
        private :
#ifdef OS_WINDOWS
                //
#else
                static std::string get_sysctl ( const std::string&  key ) {
                        char result[256];
                        size_t size = 256;
                        sysctlbyname( key.c_str(), &result[0], &size, NULL, 0 );
                        return std::string( result );
                }
                static double get_sysctl_double ( const std::string&  key ) {
                        uint64_t result;
                        size_t size = sizeof( result );
                        sysctlbyname( key.c_str(), &result, &size, NULL, 0 );
                        return static_cast<double>( result );
                }
                static int get_sysctl_int ( const std::string&  key ) {
                        int result;
                        size_t size = sizeof( result );
                        sysctlbyname( key.c_str(), &result, &size, NULL, 0 );
                        return result;
                }
#endif
        public:
                static void print ( std::ostream&  out ) {
                        out<<"date: "<<mi::SystemInfo::getDate()<<std::endl;
                        out<<"cpu: "<<mi::SystemInfo::getCpuName()<<"("<<mi::SystemInfo::getNumCores()<<"cores)"<<std::endl;
                        out<<"memory:"<<mi::SystemInfo::getMemorySize()<<"[gb]"<<std::endl;
                }

                /**
                 * @enum SIZE_TYPE Size type.
                 */
                enum SIZE_TYPE {
                        MI_BYTE,      ///< Bytes.
                        MI_KILO_BYTE, ///< Kilo bytes.
                        MI_MEGA_BYTE, ///< Mega bytes.
                        MI_GIGA_BYTE  ///< Giga bytes.
                };

                /**
                 * @brief Get peak memory usage of the program.
                 * @param [in] type Unit of memory usage.
                 * @return Peak memory usage.
                 * @sa SIZE_TYPE
                 */
                /**
                 * @brief Get peak memory usage of the program.
                 * @param [in] type Unit of memory usage.
                 * @return Peak memory usage.
                 * @sa SIZE_TYPE
                 */
                static double getPeakMemorySize( const SIZE_TYPE type = MI_MEGA_BYTE ) {
                        double peakMemory = 0;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)// Win32 API
#ifndef PEAK_MEMORY_COUNTER_DISABLED
                        PROCESS_MEMORY_COUNTERS pmc = { 0 };
                        HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId() );
                        if ( GetProcessMemoryInfo( hProcess, &pmc, sizeof( pmc ) ) ) {
                                peakMemory = static_cast<double>( pmc.PeakWorkingSetSize );
                        }
                        CloseHandle( hProcess );
#endif// PEAK_MEMORY_COUNTER_DISABLED
#else // MAC or Linux
#ifndef PEAK_MEMORY_COUNTER_DISABLED
                        ///@todo The result somewhat strange on Mac.
                        struct rusage rusage;
                        getrusage( RUSAGE_SELF, &rusage );
                        peakMemory = static_cast<double>( rusage.ru_maxrss );
#endif // PEAK_MEMORY_COUNTER_DISABLED
#endif // WIN32
                        if ( type == MI_GIGA_BYTE ) peakMemory /= 1024 * 1024 * 1024 ;
                        else if ( type == MI_MEGA_BYTE ) peakMemory /= 1024 * 1024;
                        else if ( type == MI_KILO_BYTE ) peakMemory /= 1024;
                        return  peakMemory;
                }
        };
}
#endif// MI_SYSTEM_INFO_HPP
