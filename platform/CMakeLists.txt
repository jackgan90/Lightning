project(platform)

set(WINDOW_SYSTEM_HEADERS	iwindow.h
							winwindow.h
							windowmanager.h)
set(WINDOW_SYSTEM_SOURCES	winwindow.cpp
							windowmanager.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(WIN32)
	set(PROJECT_COMPILE_FLAGS "/wd4251")
endif()

source_group("window" FILES 
			${WINDOW_SYSTEM_HEADERS}
			${WINDOW_SYSTEM_SOURCES})

if (WIN32)
	add_definitions("-DLIGHTNINGGE_WIN32")
endif()

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS})

include_directories("../foundation" "../utilities")
add_library(${PROJECT_NAME} SHARED ${WINDOW_SYSTEM_HEADERS} ${WINDOW_SYSTEM_SOURCES})
add_dependencies(${PROJECT_NAME} foundation)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS})

