cmake_minimum_required(VERSION 3.10)

project(chttpd)

set(CMAKE_C_STANDARD 99)

# include dir
include_directories(${CMAKE_SOURCE_DIR}/lib/cJSON)
include_directories(${CMAKE_SOURCE_DIR}/lib/tomlc99)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/lib/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -finput-charset=UTF-8")
set(POSTGRESQL_INCLUDE_DIR "C:/Program Files/PostgreSQL/16/include")
set(POSTGRESQL_LIB_DIR "C:/Program Files/PostgreSQL/16/lib")
set(TOML_SRC_DIR "${CMAKE_SOURCE_DIR}/lib/tomlc99")
file(GLOB TOML_SOURCES "${TOML_SRC_DIR}/*.c")
add_library(toml STATIC ${TOML_SOURCES})

if(MSVC)
    # 使用Visual Studio编译器时的设置
    add_compile_options("/utf-8")
else()
    # 使用GCC编译器时的设置
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -finput-charset=UTF-8 -fexec-charset=UTF-8")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/cJSON)
include_directories(${PROJECT_SOURCE_DIR}/lib/VS2022/Debug-dll)
include_directories(${POSTGRESQL_INCLUDE_DIR})

# link dir
link_directories("C:/Program Files/PostgreSQL/16/lib")
link_directories(${PROJECT_SOURCE_DIR}/lib/VS2022/Debug-dll)

find_library(POSTGRESQL_LIB NAMES pq PATHS ${POSTGRESQL_LIB_DIR})

# OpenSSL paths
set(OPENSSL_ROOT_DIR "${PROJECT_SOURCE_DIR}/lib/vcpkg/packages/openssl_x64-windows")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include/openssl")
set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libcrypto.lib")
set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libssl.lib")

# find OpenSSL
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

# search sourcefile
file(GLOB SRCFILES_ALL
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

# 设置源文件
set(SRC_FILES
    src/main.c
    ${SRCFILES_ALL}
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} )
target_link_libraries(${PROJECT_NAME} ${POSTGRESQL_LIB} ${PROJECT_SOURCE_DIR}/lib/VS2022/Debug-dll/libmicrohttpd-dll_d.lib ws2_32 wsock32 mswsock toml cjson)
