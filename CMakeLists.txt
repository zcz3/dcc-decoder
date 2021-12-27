cmake_minimum_required(VERSION 3.10)

# set the project name
project(dccd)

find_package(ZLIB REQUIRED)

# add the executable
add_executable(
    dccd
    src/main.c
    src/base45.c
    src/comp.c
    src/cbor.c
)

target_link_libraries(dccd ZLIB::ZLIB)
