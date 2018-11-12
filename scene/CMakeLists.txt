project(scene)
message(STATUS "Configuring project scene")

set(HEADERS sceneexportdef.h 
			scene.h 
			scenemanager.h
			drawable.h
			primitive.h
			camera.h
			scenerenderdata.h)

set(SOURCES scene.cpp 
			scenemanager.cpp
			primitive.cpp
			camera.cpp
			scenerenderdata.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
	add_definitions(-DLIGHTNING_WIN32)
endif()

define_dll_export_macro(SCENE)
include_directories(${CMAKE_SOURCE_DIR}/render
					${CMAKE_SOURCE_DIR}/render/types
					${CMAKE_SOURCE_DIR}/render/shader
					${CMAKE_SOURCE_DIR}/window
					${CMAKE_SOURCE_DIR}/entities
					${CMAKE_SOURCE_DIR}/foundation
					${CMAKE_SOURCE_DIR}/foundation/math
					${CMAKE_SOURCE_DIR}/utilities
					${Boost_INCLUDE_DIR}
					${TBB_INCLUDE_DIR}
					${RTTR_INCLUDE_DIR}
					${LIGHTNING_DEPENDENCIES_DIR}/spdlog/include)

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()

add_dependencies(${PROJECT_NAME} foundation utilities render)
target_link_libraries(${PROJECT_NAME} foundation utilities render)
