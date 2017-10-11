project(utilities)
message(STATUS "Configuring project utilities")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(HEADERS utilitiesexportdef.h
			enginealgo.h
			hashableobject.h)
set(SOURCES enginealgo.cpp)


include_directories(${CMAKE_SOURCE_DIR}/foundation
					${CMAKE_SOURCE_DIR}/foundation/memory
					${CMAKE_CURRENT_SOURCE_DIR}
					${LIGHTNINGGE_DEPENDENCIES_DIR}/boost)

if (WIN32)
	add_definitions(-DLIGHTNINGGE_WIN32
					-DMEMORY_PLATFORM_WIN32)
endif()
if(LIGHTNINGGE_USE_D3D12)
	add_definitions(-DLIGHTNINGGE_USE_D3D12)
endif()
define_dll_export_macro(UTILITIES)

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
add_dependencies(${PROJECT_NAME} foundation)
target_link_libraries(${PROJECT_NAME} foundation)
