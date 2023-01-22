add_custom_target(boot.bin ALL
    ${CMAKE_ASM_NASM_COMPILER} ${CMAKE_CURRENT_LIST_DIR}/boot.asm -f bin -o ${CMAKE_BINARY_DIR}/boot.bin
    BYPRODUCTS ${CMAKE_BINARY_DIR}/boot.bin
    SOURCES boot.asm
)
