project(window)
message(STATUS "Configuring project window")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(HEADERS	windowexportdef.h
			iwindow.h
			iwindownativehandle.h
			windowmanager.h)

set(SOURCES	winwindow.cpp
			windowmanager.cpp)

if(WIN32)
	list(APPEND HEADERS	winwindow.h
						winwindownativehandle.h)
	list(APPEND SOURCES winwindow.cpp
						winwindownativehandle.cpp)
	set(PROJECT_COMPILE_FLAGS "/wd4251")
	add_definitions(-DLIGHTNINGGE_WIN32)
endif()

define_dll_export_macro(WINDOW)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS})

include_directories(${CMAKE_SOURCE_DIR}/foundation
					${CMAKE_SOURCE_DIR}/utilities
					${CMAKE_SOURCE_DIR}/utilities/logging)

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
add_dependencies(${PROJECT_NAME} foundation utilities)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS})
target_link_libraries(${PROJECT_NAME} foundation utilities)

