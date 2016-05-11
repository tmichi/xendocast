ADD_EXECUTABLE(xendocast xendocast_main.cpp ExtractEndocastCommand.hpp ExtractEndocastCommand.cpp BinarizationRoutine.hpp WatershedRoutine.hpp)
TARGET_LINK_LIBRARIES(xendocast mibase mivol mivolut)
INSTALL(TARGETS xendocast
	RUNTIME
	DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

