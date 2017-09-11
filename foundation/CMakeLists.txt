project(foundation)

set(HEADERS	common.h)
set(SOURCES common.cpp)

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
