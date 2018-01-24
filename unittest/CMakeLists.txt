project(unittest)
message(STATUS "Configuring project test")

set(HEADERS catch.hpp)
set(SOURCES main.cpp
			memorytest.cpp
			mathtest.cpp)

if (WIN32)
	add_definitions(-DLIGHTNINGGE_WIN32)
endif()
if(LIGHTNINGGE_USE_D3D12)
	add_definitions(-DLIGHTNINGGE_USE_D3D12)
endif()

add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED)

include_directories( ${CMAKE_SOURCE_DIR}/foundation
					${CMAKE_SOURCE_DIR}/render/types
					${CMAKE_SOURCE_DIR}/foundation/memory
					${LIGHTNINGGE_DEPENDENCIES_DIR}/eigen
					${LIGHTNINGGE_DEPENDENCIES_DIR}/boost)
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME} WIN32 ${HEADERS} ${SOURCES})
add_dependencies(${PROJECT_NAME} foundation)
target_link_libraries(${PROJECT_NAME} foundation)
if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()
