#ifndef EXTRACT_ENDOCAST_COMMAND_HPP
#define EXTRACT_ENDOCAST_COMMAND_HPP 1
#include <mi/CommandTemplate.hpp>
#include <mi/VolumeData.hpp>
#include <mi/Mesh.hpp>
#include <mi/FileNameConverter.hpp>
template <typename T>
class ExtractEndocastCommand : public mi::CommandTemplate
{
private:
        std::string _ct_file;
        std::string _output_file;

        int _header_size;
        mi::Point3i _size;
        mi::Vector3d _pitch;
        mi::Vector3d _origin;

        T _isovalue;
        double _scale;
        double _hole;

        mi::VolumeData<T>    _ctData;
        mi::VolumeData<char> _binaryData;

        mi::Mesh _endocast_polygon;

        bool _auto;
        bool _fillHole;
        int _num_threads;

public:
        ExtractEndocastCommand ( void ) ;
        ~ExtractEndocastCommand ( void ) ;

        bool init ( const mi::Argument& arg );
        bool run  ( void ) ;
        bool term ( void ) ;
private:
        bool binarize ( const mi::VolumeData<T>& ctData, mi::VolumeData<char>& binaryData ) ;
        bool watershed( mi::VolumeData<float>& distData, mi::VolumeData<char>& labelData ) ;
        bool polygonize_endocast( mi::VolumeData<char>& labelData );

        std::string create_file_name ( const std::string& tail, const std::string& ext );
};

#endif///EXTRACT_ENDOCAST_COMMAND_HPP
