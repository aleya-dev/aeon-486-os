add_executable(kernel.bin
    entry.asm
    main.c
    drivers/display/display.c
    drivers/display/display.h
    drivers/display/textmode.c
    drivers/display/textmode.h
    platform/i386/hal.c
    platform/i386/hal.h
    lib/stdlib.c
    lib/string.c
    include/aeon/file.h
    include/aeon/memory.h
    include/aeon/types.h
    include/aeon/stdlib.h
    include/aeon/string.h
    include/aeon/console.h
)

set_target_properties(kernel.bin PROPERTIES
    LINKER_LANGUAGE C
    ASM_NASMFLAGS "-f elf"
    CFLAGS "-ffreestanding"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_include_directories(kernel.bin
    PUBLIC
        include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
