project(platform)
message(STATUS "Configuring project platform")

set(APPLICATION_SUBSYSTEM_DIR "application")



set(APPLICATION_HEADERS		${APPLICATION_SUBSYSTEM_DIR}/iapplication.h
							${APPLICATION_SUBSYSTEM_DIR}/appfactory.h)
set(APPLICATION_SOURCES		${APPLICATION_SUBSYSTEM_DIR}/application.cpp
							${APPLICATION_SUBSYSTEM_DIR}/appfactory.cpp)

if(WIN32)
	list(APPEND APPLICATION_HEADERS ${APPLICATION_SUBSYSTEM_DIR}/win32application.h)
	list(APPEND APPLICATION_SOURCES ${APPLICATION_SUBSYSTEM_DIR}/win32application.cpp)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(WIN32)
	set(PROJECT_COMPILE_FLAGS "/wd4251")
	add_definitions(-DLIGHTNINGGE_WIN32)
endif()

source_group("application" FILES 
			${APPLICATION_HEADERS}
			${APPLICATION_SOURCES})

set(HEADERS platformexportdef.h)
list(APPEND HEADERS ${APPLICATION_HEADERS})
set(SOURCES ${APPLICATION_SOURCES})

define_dll_export_macro(PLATFORM)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS})

include_directories(${CMAKE_SOURCE_DIR}/foundation
					${CMAKE_SOURCE_DIR}/utilities
					${CMAKE_SOURCE_DIR}/utilities/logging
					${CMAKE_CURRENT_SOURCE_DIR} 
					${CMAKE_SOURCE_DIR}/renderer
					${CMAKE_SOURCE_DIR}/window)

include_directories(${LIGHTNINGGE_DEPENDENCIES_DIR}/boost)
add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
add_dependencies(${PROJECT_NAME} foundation utilities window renderer)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS})
target_link_libraries(${PROJECT_NAME} foundation utilities window renderer)

