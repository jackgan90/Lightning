project(ResourceGenerator)
message(STATUS "Configuring project ResourceGenerator")

set(HEADERS )
set(SOURCES ResourceGenerator.cpp)

set(ASSIMP_ROOT_DIR ${LIGHTNING_DEPENDENCIES_DIR}/assimp)
find_package(assimp REQUIRED)

include_directories(${ASSIMP_ROOT_DIR}/include)


if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd /MP")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD /MP")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc /MP")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc /MP")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME} WIN32 ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIBRARY})

if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()
