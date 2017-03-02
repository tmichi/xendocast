ADD_EXECUTABLE(fillporosity 
			fill_porosity_main.cpp PorosityAnalyzer.hpp PorosityAnalyzer.cpp ConstrainedMorphology.hpp ConstrainedMorphology.cpp FillPorosityCommand.hpp FillPorosityCommand.cpp)

INCLUDE_DIRECTORIES(../)
INSTALL(TARGETS fillporosity 
	RUNTIME
	DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

