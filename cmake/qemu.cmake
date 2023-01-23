include_guard()

find_program(QEMU_EXECUTABLE qemu-system-i386)

if (QEMU_EXECUTABLE)
    message(STATUS "Found qemu: ${QEMU_EXECUTABLE}")
    message(STATUS "Enabling qemu target.")

    set(QEMU_FOUND ON CACHE BOOL "")

    add_custom_target(qemu
        ${QEMU_EXECUTABLE} -gdb tcp::9999 -cpu 486 -m size=4M,maxmem=4M ${CMAKE_BINARY_DIR}/floppy.img
    )

    add_dependencies(qemu
        floppy
    )
else ()
    message(WARNING "qemu not found.")
endif ()
