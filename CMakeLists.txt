cmake_minimum_required(VERSION 3.1)

project(aeon_486_os C CXX ASM_NASM)

add_subdirectory(boot)
add_subdirectory(kernel)

add_custom_target(floppy ALL
    COMMAND bash -c \"dd if=/dev/zero of=${CMAKE_BINARY_DIR}/floppy.img bs=512 count=2880\"
    COMMAND bash -c \"dd if=${CMAKE_BINARY_DIR}/boot.bin of=${CMAKE_BINARY_DIR}/floppy.img conv=notrunc\"
    COMMAND bash -c \"dd conv=notrunc if=${CMAKE_BINARY_DIR}/kernel.bin of=${CMAKE_BINARY_DIR}/floppy.img seek=512 bs=1\"
    BYPRODUCTS ${CMAKE_BINARY_DIR}/floppy.img
)

add_dependencies(floppy
    boot.bin
    kernel.bin
)
