cmake_minimum_required(VERSION 3.1)

project(aeon_486_os C CXX ASM ASM_NASM)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake")
include(qemu)

add_subdirectory(boot)
add_subdirectory(kernel)

add_custom_target(image ALL
    COMMAND bash -c \"dd if=/dev/zero of=${CMAKE_BINARY_DIR}/hdd.img bs=512 count=40000\"
    COMMAND bash -c \"dd if=${CMAKE_BINARY_DIR}/boot.bin of=${CMAKE_BINARY_DIR}/hdd.img conv=notrunc\"
    COMMAND bash -c \"dd conv=notrunc if=${CMAKE_BINARY_DIR}/kernel.bin of=${CMAKE_BINARY_DIR}/hdd.img seek=512 bs=1\"
    BYPRODUCTS ${CMAKE_BINARY_DIR}/hdd.img
)

add_dependencies(image
    boot.bin
    kernel.bin
)
