SET(CMAKE_C_COMPILER "D:/clang/llvm-mingw-20240518-msvcrt-x86_64/bin/gcc.exe")
cmake_minimum_required(VERSION 3.10)

project(chttpd)

set(CMAKE_C_STANDARD 99)

# include dir
# include_directories(${CMAKE_SOURCE_DIR}/lib/libmicrohttpd-1.0.1/src/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/cJSON)

set(POSTGRESQL_INCLUDE_DIR "C:/Program Files/PostgreSQL/16/include")
set(POSTGRESQL_LIB_DIR "C:/Program Files/PostgreSQL/16/lib")
include_directories(${PROJECT_SOURCE_DIR}/lib/VS2022/Debug-dll)
include_directories(${POSTGRESQL_INCLUDE_DIR})
# link dir
link_directories("C:/Program Files/PostgreSQL/16/lib")
link_directories(${PROJECT_SOURCE_DIR}/lib/VS2022/Debug-dll)
# link_directories(${CMAKE_SOURCE_DIR}/lib/libmicrohttpd-1.0.1/src/lib)
find_library(POSTGRESQL_LIB NAMES pq PATHS ${POSTGRESQL_LIB_DIR})

# search sourcefile
file(GLOB SRCFILES_ALL
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

# 设置源文件
set(SRC_FILES
    src/main.c
    ${SRCFILES_ALL}
)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${POSTGRESQL_LIB} ${PROJECT_SOURCE_DIR}/lib/VS2022/Debug-dll/libmicrohttpd-dll_d.lib ws2_32 wsock32 mswsock)
