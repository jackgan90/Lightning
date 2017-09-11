project(foundation)

set(HEADERS	common.h)
set(SOURCES common.cpp)

set(PROJECT_COMPILE_FLAGS "/wd4251")

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS})
