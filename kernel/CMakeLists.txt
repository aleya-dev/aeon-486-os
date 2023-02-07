add_executable(kernel.bin
    boot.s
    main.c
    drivers/block/ata.c
    drivers/block/ata.h
    drivers/display/display.c
    drivers/display/display.h
    drivers/display/textmode.c
    drivers/display/textmode.h
    platform/i386/hal.c
    platform/i386/hal.h
    platform/i386/idt.c
    platform/i386/idt.h
    platform/i386/idt.s
    platform/i386/gdt.c
    platform/i386/gdt.h
    platform/i386/irq.c
    platform/i386/irq.h
    lib/mutex.c
    lib/hexdump.c
    lib/hexdump.h
    include/aeon/file.h
    include/aeon/console.h
    include/aeon/mutex.h
)

set_target_properties(kernel.bin PROPERTIES
    LINKER_LANGUAGE C
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_link_options(kernel.bin
    PRIVATE
        -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)

target_link_libraries(kernel.bin
    PRIVATE
        klibc
)

target_include_directories(kernel.bin
    PUBLIC
        include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
