project(Window)
message(STATUS "Configuring project Window")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(HEADERS	IWindow.h
			GameWindow.h
			WindowsGameWindow.h
			WindowEvents.h)

set(SOURCES	GameWindow.cpp
			WindowsGameWindow.cpp)

set(PLUGIN_HEADERS IWindowPlugin.h)
set(PLUGIN_SOURCES WindowPluginImpl.cpp)

if(WIN32)
	add_definitions(-DLIGHTNING_WIN32)
endif()
if(LIGHTNING_USE_D3D12)
	add_definitions(-DLIGHTNING_USE_D3D12)
endif()

list(APPEND HEADERS ${PLUGIN_HEADERS})
list(APPEND SOURCES ${PLUGIN_SOURCES})

source_group("Plugin" FILES ${PLUGIN_HEADERS} ${PLUGIN_SOURCES})

define_dll_export_macro(WINDOW)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS})

include_directories(${CMAKE_SOURCE_DIR}/PluginSystem
					${CMAKE_SOURCE_DIR}/Foundation
					${CMAKE_SOURCE_DIR}/Foundation/Memory
					${Boost_INCLUDE_DIR}
					${TBB_INCLUDE_DIR}
					${RTTR_INCLUDE_DIR}
					${SPDLOG_INCLUDE_DIR})

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
add_dependencies(${PROJECT_NAME} Foundation)
target_link_libraries(${PROJECT_NAME} ${TBB_LIBRARIES})

if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd /MP")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD /MP")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc /MP")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc /MP")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()

