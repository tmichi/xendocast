#ifndef FILL_POROSITY_COMMAND_HPP
#define FILL_POROSITY_COMMAND_HPP 1
#include <mi/CommandTemplate.hpp>
#include <mi/VolumeData.hpp>
#include <mi/FileNameConverter.hpp>

template <typename T>
class FillPorosityCommand : public mi::CommandTemplate
{
private:
        std::string _ct_file;
        std::string _output_file;

        int _header_size;
        mi::Point3i _size;
        mi::Vector3d _pitch;

        double _isovalue;
        double _radius;

        int _num_pruning;
        int _num_growing;

        mi::VolumeData<T>    _ctData;
        int _num_threads;

public:
        FillPorosityCommand ( void ) ;
        ~FillPorosityCommand ( void ) ;

        bool init ( const mi::Argument& arg );
        bool run  ( void ) ;
        bool term ( void ) ;
private:
        void closing ( mi::VolumeData<char>& inData, mi::VolumeData<char>& outData , const double radius );
        void fill_closed_porosity ( mi::VolumeData<char>& inData, mi::VolumeData<char>& outData );
        void constrained_opening ( mi::VolumeData<char>& inData, mi::VolumeData<char>& outData, const int num_opening, const int num_closing );
};

#endif///FILL_POROSITY_COMMAND_HPP
