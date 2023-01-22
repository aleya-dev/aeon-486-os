add_executable(kernel.bin
    #entry.asm
    main.c
)

set_target_properties(kernel.bin PROPERTIES
    LINKER_LANGUAGE C
    ASM_NASMFLAGS "-f elf"
    CFLAGS "-ffreestanding"
    EXE_LINKER_FLAGS "-Ttext 0x1000 --oformat binary"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
