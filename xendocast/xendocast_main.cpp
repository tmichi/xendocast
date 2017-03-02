#include "ExtractEndocastCommand.hpp"
template<typename T>
int exec ( mi::Argument& arg )
{
	std::cerr<<"size:"<<sizeof(T)<<std::endl;
        ExtractEndocastCommand<T> cmd;
        return mi::CommandTemplate::execute( cmd,arg );
}

int main ( int argc, char** argv )
{
        mi::Argument arg( argc, argv );
        if( arg.exist( "-char" )   ) return exec<char>( arg );
        if( arg.exist( "-uchar" )  ) return exec<unsigned char>( arg );
        if( arg.exist( "-short" )  ) return exec<short>( arg );
        if( arg.exist( "-ushort" ) ) return exec<unsigned short>( arg );
        if( arg.exist( "-int" )    ) return exec<int>( arg );
        if( arg.exist( "-uint" )   ) return exec<unsigned int>( arg );
        if( arg.exist( "-float" )   ) return exec<float>( arg );
        if( arg.exist( "-double" )   ) return exec<double>( arg );
        std::cerr<<"type '"<<argv[0]<<" -char, -uchar, -short, -ushort, -int, -uint, -float or -double'"<<std::endl;
}



