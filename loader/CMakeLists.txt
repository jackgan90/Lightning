include(helpers)

project(loader)
message(STATUS "Configuring project loader")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(HEADERS loaderexportdef.h
			iserializer.h
			loader.h)
set(SOURCES loader.cpp)

if (WIN32)
	add_definitions(-DLIGHTNING_WIN32)
endif()

define_dll_export_macro(LOADER)


include_directories(${CMAKE_SOURCE_DIR}/foundation 
					${Boost_INCLUDE_DIR}
					${TBB_INCLUDE_DIR}
					${SPDLOG_INCLUDE_DIR})
add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
add_dependencies(${PROJECT_NAME} foundation)
target_link_libraries(${PROJECT_NAME} foundation)
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
