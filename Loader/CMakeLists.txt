include(helpers)

project(Loader)
message(STATUS "Configuring project Loader")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(PLUGIN_HEADERS	ILoaderPlugin.h)
set(PLUGIN_SOURCES	LoaderPluginImpl.cpp)

set(INTERFACE_HEADERS	ISerializer.h
						ILoader.h
						ISerializeBuffer.h)

set(HEADERS SerializeBuffer.h
			Loader.h)

set(SOURCES Loader.cpp
			SerializeBuffer.cpp)

list(APPEND HEADERS ${PLUGIN_HEADERS} ${INTERFACE_HEADERS})
list(APPEND SOURCES ${PLUGIN_SOURCES})

if (WIN32)
	add_definitions(-DLIGHTNING_WIN32)
endif()

source_group("Interfaces" FILES ${INTERFACE_HEADERS})

source_group("Plugin" FILES
			${PLUGIN_HEADERS}
			${PLUGIN_SOURCES})

define_dll_export_macro(LOADER)


include_directories(${CMAKE_SOURCE_DIR}/PluginSystem
					${CMAKE_SOURCE_DIR}/Foundation 
					${Boost_INCLUDE_DIR}
					${TBB_INCLUDE_DIR}
					${SPDLOG_INCLUDE_DIR})
add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
add_dependencies(${PROJECT_NAME} Foundation)
target_link_libraries(${PROJECT_NAME} ${TBB_LIBRARIES})
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4250 /wd4251 /wd4275 /EHsc /MDd /MP")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4250 /wd4251 /wd4275 /EHsc /MD /MP")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4250 /wd4251 /wd4275 /EHsc /MP")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4250 /wd4251 /wd4275 /EHsc /MP")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
