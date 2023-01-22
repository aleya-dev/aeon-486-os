#add_executable(boot.bin
#    boot.asm
#)
#
##nasm boot/boot.asm -f bin -o ${output_dir}/boot.bin
#
#
#set_target_properties(boot.bin PROPERTIES
#    LINKER_LANGUAGE ASM
#    ASM_NASMFLAGS "-f bin"
#    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
#)

add_custom_target(boot.bin
    ${CMAKE_ASM_NASM_COMPILER} ${CMAKE_CURRENT_LIST_DIR}/boot.asm -f bin -o ${CMAKE_BINARY_DIR}/boot.bin
    BYPRODUCTS ${CMAKE_BINARY_DIR}/boot.bin
    SOURCES boot.asm
)
