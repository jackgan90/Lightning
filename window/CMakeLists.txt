project(window)
message(STATUS "Configuring project window")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(HEADERS	windowexportdef.h
			iwindow.h
			window.h
			windowmanager.h
			windowmessage.h)

set(SOURCES	window.cpp
			winwindow.cpp
			windowmanager.cpp)

if(WIN32)
	list(APPEND HEADERS	winwindow.h)
	list(APPEND SOURCES winwindow.cpp)
	add_definitions(-DLIGHTNINGGE_WIN32
					-DMEMORY_PLATFORM_WIN32)
endif()
if(LIGHTNINGGE_USE_D3D12)
	add_definitions(-DLIGHTNINGGE_USE_D3D12)
endif()

define_dll_export_macro(WINDOW)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS})

include_directories(${LIGHTNINGGE_DEPENDENCIES_DIR}/boost
					${LIGHTNINGGE_DEPENDENCIES_DIR}/eigen
					${CMAKE_SOURCE_DIR}/platform
					${CMAKE_SOURCE_DIR}/platform/application
					${CMAKE_SOURCE_DIR}/renderer
					${CMAKE_SOURCE_DIR}/renderer/shader
					${CMAKE_SOURCE_DIR}/renderer/types
					${CMAKE_SOURCE_DIR}/foundation
					${CMAKE_SOURCE_DIR}/utilities
					${CMAKE_SOURCE_DIR}/foundation/memory)

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
add_dependencies(${PROJECT_NAME} foundation)

if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
target_link_libraries(${PROJECT_NAME} foundation)

