project(Scene)
message(STATUS "Configuring project Scene")

set(HEADERS IScene.h
			Scene.h 
			ISceneManager.h
			SceneManager.h
			IDrawable.h
			IPrimitive.h
			Primitive.h
			ICamera.h
			Camera.h
			SceneRenderData.h)

set(SOURCES Scene.cpp 
			SceneManager.cpp
			Primitive.cpp
			Camera.cpp
			SceneRenderData.cpp)

set(PLUGIN_HEADERS	ScenePlugin.h)
set(PLUGIN_SOURCES	ScenePlugin.cpp)

list(APPEND HEADERS	${PLUGIN_HEADERS})
list(APPEND SOURCES ${PLUGIN_SOURCES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

source_group("Plugin" ${PLUGIN_HEADERS} ${PLUGIN_SOURCES})

if (WIN32)
	add_definitions(-DLIGHTNING_WIN32)
endif()

define_dll_export_macro(SCENE)
include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/Render
					${CMAKE_SOURCE_DIR}/Render/Types
					${CMAKE_SOURCE_DIR}/Render/Shader
					${CMAKE_SOURCE_DIR}/Window
					${CMAKE_SOURCE_DIR}/Entities
					${CMAKE_SOURCE_DIR}/Foundation
					${CMAKE_SOURCE_DIR}/Foundation/Math
					${Boost_INCLUDE_DIR}
					${TBB_INCLUDE_DIR}
					${RTTR_INCLUDE_DIR}
					${SPDLOG_INCLUDE_DIR})

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()

add_dependencies(${PROJECT_NAME} Foundation Render)
target_link_libraries(${PROJECT_NAME} Foundation Render)
