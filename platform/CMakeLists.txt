project(platform)

set(WINDOW_SYSTEM_HEADERS	iwindow.h
							winwindow.h
							windowmanager.h)
set(WINDOW_SYSTEM_SOURCES	winwindow.cpp
							windowmanager.cpp)

set(PROJECT_COMPILE_FLAGS "/wd4251")

source_group("window" FILES 
			${WINDOW_SYSTEM_HEADERS}
			${WINDOW_SYSTEM_SOURCES})

if (WIN32)
	add_definitions("-DLIGHTNINGGE_WIN32")
endif()

set(PLATFORM_LINKER_FLAGS "/MD")

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS})

include_directories(../foundation)
add_library(${PROJECT_NAME} SHARED ${WINDOW_SYSTEM_HEADERS} ${WINDOW_SYSTEM_SOURCES})
add_dependencies(${PROJECT_NAME} foundation)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS})

