project(utilities)
message(STATUS "Configuring project utilities")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(HEADERS utilitiesexportdef.h)


include_directories(${CMAKE_SOURCE_DIR}/foundation
					${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
	add_definitions(-DLIGHTNINGGE_WIN32)
endif()
define_dll_export_macro(UTILITIES)

#add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
if(MSVC)
	#set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/wd4251")
	#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /DDEBUG")
	#set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
#add_dependencies(${PROJECT_NAME} foundation)
#target_link_libraries(${PROJECT_NAME} foundation)
