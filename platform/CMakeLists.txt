project(platform)
message(STATUS "Configuring project platform")

set(WINDOW_SUBSYSTEM_DIR "window")
set(APPLICATION_SUBSYSTEM_DIR "application")

set(WINDOW_SYSTEM_HEADERS	${WINDOW_SUBSYSTEM_DIR}/iwindow.h
							${WINDOW_SUBSYSTEM_DIR}/iwindownativehandle.h
							${WINDOW_SUBSYSTEM_DIR}/windowmanager.h)

set(WINDOW_SYSTEM_SOURCES	${WINDOW_SUBSYSTEM_DIR}/winwindow.cpp
							${WINDOW_SUBSYSTEM_DIR}/windowmanager.cpp)


set(APPLICATION_HEADERS		${APPLICATION_SUBSYSTEM_DIR}/iapplication.h
							${APPLICATION_SUBSYSTEM_DIR}/appfactory.h)
set(APPLICATION_SOURCES		${APPLICATION_SUBSYSTEM_DIR}/application.cpp
							${APPLICATION_SUBSYSTEM_DIR}/appfactory.cpp)

if(WIN32)
	list(APPEND WINDOW_SYSTEM_HEADERS ${WINDOW_SUBSYSTEM_DIR}/winwindow.h
									  ${WINDOW_SUBSYSTEM_DIR}/winwindownativehandle.h)
	list(APPEND WINDOW_SYSTEM_HEADERS ${WINDOW_SUBSYSTEM_DIR}/winwindow.cpp
									  ${WINDOW_SUBSYSTEM_DIR}/winwindownativehandle.cpp)
	list(APPEND APPLICATION_HEADERS ${APPLICATION_SUBSYSTEM_DIR}/win32application.h)
	list(APPEND APPLICATION_SOURCES ${APPLICATION_SUBSYSTEM_DIR}/win32application.cpp)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(WIN32)
	set(PROJECT_COMPILE_FLAGS "/wd4251")
	add_definitions(-DLIGHTNINGGE_WIN32)
endif()

source_group("window" FILES 
			${WINDOW_SYSTEM_HEADERS}
			${WINDOW_SYSTEM_SOURCES})

source_group("application" FILES 
			${APPLICATION_HEADERS}
			${APPLICATION_SOURCES})

set(HEADERS platformexportdef.h)
list(APPEND HEADERS ${WINDOW_SYSTEM_HEADERS} ${APPLICATION_HEADERS})
set(SOURCES ${WINDOW_SYSTEM_SOURCES} ${APPLICATION_SOURCES})

define_dll_export_macro(PLATFORM)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS})

include_directories(${CMAKE_SOURCE_DIR}/foundation
					${CMAKE_SOURCE_DIR}/utilities
					${CMAKE_SOURCE_DIR}/utilities/logging
					${CMAKE_CURRENT_SOURCE_DIR} 
					${CMAKE_CURRENT_SOURCE_DIR}/window)

include_directories(${LIGHTNINGGE_DEPENDENCIES_DIR}/boost)
add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
add_dependencies(${PROJECT_NAME} foundation utilities)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS})
target_link_libraries(${PROJECT_NAME} foundation utilities)

