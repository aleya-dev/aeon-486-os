add_library(klibc OBJECT
    private/ctype.c
    private/memory.c
    private/stdlib.c
    private/string.c
    public/klibc/ctype.h
    public/klibc/memory.h
    public/klibc/stdlib.h
    public/klibc/string.h
    public/klibc/types.h
    public/klibc/stddef.h
)

set_target_properties(klibc PROPERTIES
    LINKER_LANGUAGE C
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_include_directories(klibc
    PUBLIC
        public
)
