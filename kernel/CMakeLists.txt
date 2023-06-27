add_executable(kernel.bin
    boot.s
    main.c
    multiboot.h
    kmodule.h
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
    memory/memory.c
    memory/memory.h
    memory/paging.c
    memory/paging.h
    lib/mutex.c
    lib/hexdump.c
    lib/hexdump.h
    fs/devfs.c
    fs/devfs.h
    fs/file.cpp
    include/aeon/console.h
    include/aeon/mutex.h
    include/aeon/file.hpp
    include/aeon/seek_direction.hpp
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
