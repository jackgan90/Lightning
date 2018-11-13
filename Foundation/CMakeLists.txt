project(Foundation)
message(STATUS "Configuring project Foundation")

set(MEMORY_HEADERS	Memory/IMemoryAllocator.h
					Memory/PoolAllocator.h
					Memory/StackAllocator.h
					Memory/FrameMemoryAllocator.h
					Memory/MemoryPool.h)

set(MEMORY_SOURCES	Memory/FrameMemoryAllocator.cpp)

set(MATH_HEADERS	Math/Common.h
					Math/Vector.h
					Math/Matrix.h
					Math/Quaternion.h
					Math/Transform.h)

set(MATH_SOURCES	Math/Transform.cpp)

set(ECS_HEADERS		ECS/Entity.h
					ECS/Component.h
					ECS/System.h
					ECS/Event.h
					ECS/EventManager.h
					ECS/EntityManager.h
					ECS/SystemManager.h)

set(ECS_SOURCES		ECS/Component.cpp
					ECS/SystemManager.cpp
					ECS/EntityManager.cpp
					ECS/EventManager.cpp)

set(HEADERS	Common.h
			TimeSystem.h
			FileSystem.h
			Logger.h
			FileSystemFactory.h
			ConfigManager.h
			FoundationExportDef.h
			Singleton.h
			Counter.h
			PlainObject.h
			MurmurHash3.h
			Timer.h
			TimerManager.h
			Container.h
			EnumOperation.h
			ThreadLocalSingleton.h
			Environment.h
			SystemPriority.h
			)
set(SOURCES Common.cpp
			FileSystem.cpp
			ConfigManager.cpp
			FileSystemFactory.cpp
			MurmurHash3.cpp
			Timer.cpp
			TimerManager.cpp
			TimeSystem.cpp
			Environment.cpp)

list(APPEND HEADERS ${MEMORY_HEADERS} ${MATH_HEADERS} ${ECS_HEADERS})
list(APPEND SOURCES ${MEMORY_SOURCES} ${MATH_SOURCES} ${ECS_SOURCES})

source_group("Memory" FILES	${MEMORY_HEADERS} ${MEMORY_SOURCES})
source_group("Math" FILES ${MATH_HEADERS} ${MATH_SOURCES})
source_group("ECS" FILES ${ECS_HEADERS} ${ECS_SOURCES})

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

include_directories(${CMAKE_SOURCE_DIR}/Foundation
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
