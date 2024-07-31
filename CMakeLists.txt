cmake_minimum_required(VERSION 3.10)
project(YourProjectName)

set(CMAKE_C_STANDARD 99)

include_directories(${CMAKE_SOURCE_DIR}/lib/libmicrohttpd-1.0.1/src/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/cJSON)

file(GLOB LIBMICROHTTPD_SRC
    "${CMAKE_SOURCE_DIR}/lib/libmicrohttpd-1.0.1/src/*.c"
)

file(GLOB CJSON_SRC 
    "${CMAKE_SOURCE_DIR}/lib/cJSON/*.c"
)

set(SRC_FILES
    src/main.c
    ${LIBMICROHTTPD_SRC}
    ${CJSON_SRC}
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
