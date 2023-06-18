
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was Config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

####################################################################################

include(CMakeFindDependencyMacro)

if(NOT TARGET IrrlichtMt::IrrlichtMt)
	# private dependency only explicitly needed with static libs
	if(ON AND NOT OFF)
		find_dependency(SDL2)
	endif()
	include("${CMAKE_CURRENT_LIST_DIR}/IrrlichtMtTargets.cmake")
endif()
