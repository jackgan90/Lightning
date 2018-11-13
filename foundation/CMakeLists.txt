project(Foundation)
message(STATUS "Configuring project foundation")

set(MEMORY_HEADERS	memory/imemoryallocator.h
					memory/poolallocator.h
					memory/stackallocator.h
					memory/framememoryallocator.h
					memory/memorypool.h)

set(MEMORY_SOURCES	memory/framememoryallocator.cpp)

set(MATH_HEADERS	math/common.h
					math/vector.h
					math/matrix.h
					math/quaternion.h
					math/transform.h)

set(MATH_SOURCES	math/transform.cpp)

set(ECS_HEADERS		ecs/entity.h
					ecs/component.h
					ecs/system.h
					ecs/event.h
					ecs/eventmanager.h
					ecs/entitymanager.h
					ecs/systemmanager.h)

set(ECS_SOURCES		ecs/component.cpp
					ecs/systemmanager.cpp
					ecs/entitymanager.cpp
					ecs/eventmanager.cpp)

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
			threadlocalsingleton.h
			environment.h
			systempriority.h
			)
set(SOURCES common.cpp
			filesystem.cpp
			configmanager.cpp
			filesystemfactory.cpp
			murmurhash3.cpp
			timer.cpp
			timermanager.cpp
			timesystem.cpp
			environment.cpp)

list(APPEND HEADERS ${MEMORY_HEADERS} ${MATH_HEADERS} ${ECS_HEADERS})
list(APPEND SOURCES ${MEMORY_SOURCES} ${MATH_SOURCES} ${ECS_SOURCES})

source_group("memory" FILES	${MEMORY_HEADERS} ${MEMORY_SOURCES})
source_group("math" FILES ${MATH_HEADERS} ${MATH_SOURCES})
source_group("ecs" FILES ${ECS_HEADERS} ${ECS_SOURCES})

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

include_directories(${CMAKE_SOURCE_DIR}/foundation
					${Boost_INCLUDE_DIR}
					${TBB_INCLUDE_DIR}
					${RTTR_INCLUDE_DIR}
					${SPDLOG_INCLUDE_DIR})

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${TBB_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${RTTR_LIBRARIES})

if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TBB_DLLS} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TBB_DLLS} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RTTR_DLLS} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RTTR_DLLS} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
