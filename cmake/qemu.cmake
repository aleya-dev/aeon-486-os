include_guard()

find_program(QEMU_EXECUTABLE qemu-system-i386)

if (QEMU_EXECUTABLE)
    message(STATUS "Found qemu: ${QEMU_EXECUTABLE}")
    message(STATUS "Enabling qemu target.")

    set(QEMU_FOUND ON CACHE BOOL "")

    add_custom_target(qemu
        ${QEMU_EXECUTABLE} -s -cpu 486 -m size=4M,maxmem=4M --kernel ${CMAKE_BINARY_DIR}/kernel.bin -no-reboot -d cpu_reset
    )

    add_dependencies(qemu
        kernel.bin
    )
else ()
    message(WARNING "qemu not found.")
endif ()
