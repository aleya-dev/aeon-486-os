include_guard()

find_program(BOCHS_EXECUTABLE bochs)

if (BOCHS_EXECUTABLE)
    message(STATUS "Found bochs: ${BOCHS_EXECUTABLE}")
    message(STATUS "Enabling bochs target.")

    set(BOCHS_FOUND ON CACHE BOOL "")
    
    set(AEON_FLOPPY_IMAGE_PATH ${CMAKE_BINARY_DIR}/floppy.img)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/settings.bxrc.in settings.bxrc @ONLY)

    add_custom_target(bochs
        ${BOCHS_EXECUTABLE} -f ${CMAKE_BINARY_DIR}/settings.bxrc
    )

    add_dependencies(bochs
        floppy
    )
else ()
    message(WARNING "Bochs not found.")
endif ()
