project(foundation)
message(STATUS "Configuring project foundation")

set(MEMORY_HEADERS	memory/imemoryallocator.h
					memory/poolallocator.h
					memory/stackallocator.h
					memory/ringallocator.h
					memory/memorypool.h)

set(MEMORY_SOURCES	memory/ringallocator.cpp)

set(MATH_HEADERS	math/common.h
					math/vector.h
					math/matrix.h
					math/quaternion.h
					math/transform.h)

set(MATH_SOURCES	math/transform.cpp)

set(HEADERS	common.h
			timesystem.h
			filesystem.h
			logger.h
			filesystemfactory.h
			configmanager.h
			foundationexportdef.h
			singleton.h
			counter.h
			plainobject.h
			murmurhash3.h
			timer.h
			timermanager.h
			container.h
			enumoperation.h
			)
set(SOURCES common.cpp
			filesystem.cpp
			logger.cpp
			configmanager.cpp
			filesystemfactory.cpp
			murmurhash3.cpp
			timer.cpp
			timermanager.cpp
			timesystem.cpp)

list(APPEND HEADERS ${MEMORY_HEADERS} ${MATH_HEADERS})
list(APPEND SOURCES ${MEMORY_SOURCES} ${MATH_SOURCES})

source_group("memory" FILES	${MEMORY_HEADERS} ${MEMORY_SOURCES})
source_group("math" FILES ${MATH_HEADERS} ${MATH_SOURCES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
	add_definitions(-DLIGHTNING_WIN32)
endif()
if(LIGHTNING_USE_D3D12)
	add_definitions(-DLIGHTNING_USE_D3D12)
endif()
add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED)

define_dll_export_macro(FOUNDATION)

include_directories(${LIGHTNING_DEPENDENCIES_DIR}/boost
					${CMAKE_SOURCE_DIR}/foundation)
add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
endif()

set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${LIGHTNING_DEPENDENCIES_DIR}/boost)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${LIGHTNING_DEPENDENCIES_DIR}/boost/lib)
set(BOOST_ROOT ${LIGHTNING_DEPENDENCIES_DIR}/boost)
find_package(Boost 1.65.1 REQUIRED)
if(Boost_FOUND)
	add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED)
endif()

if(MSVC)
	get_msvc_postfix(POSTFIX)
	target_link_libraries(${PROJECT_NAME} debug ${Boost_LIBRARY_DIRS}/libboost_filesystem-vc${POSTFIX}-mt-gd-1_65_1.lib)
	target_link_libraries(${PROJECT_NAME} debug ${Boost_LIBRARY_DIRS}/libboost_system-vc${POSTFIX}-mt-gd-1_65_1.lib)
	target_link_libraries(${PROJECT_NAME} optimized ${Boost_LIBRARY_DIRS}/libboost_filesystem-vc${POSTFIX}-mt-1_65_1.lib)
	target_link_libraries(${PROJECT_NAME} optimized ${Boost_LIBRARY_DIRS}/libboost_system-vc${POSTFIX}-mt-1_65_1.lib)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
