function(setup_install_rules)
    set(options INTERFACE)
    set(oneValueArgs TARGET NAMESPACE CONFIG_FILE_EXTRA_CONTENT INCLUDE_SUBDIR)
    set(multiValueArgs INCLUDE_DIRS)
    cmake_parse_arguments(INSTALL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    include(CMakePackageConfigHelpers)

    # 1. Install binary files (if not INTERFACE)
    if (INSTALL_INTERFACE)
        install(TARGETS ${INSTALL_TARGET}
                EXPORT ${INSTALL_TARGET}Targets
        )
    else ()
        install(TARGETS ${INSTALL_TARGET}
                EXPORT ${INSTALL_TARGET}Targets
                LIBRARY DESTINATION lib
                ARCHIVE DESTINATION lib
                RUNTIME DESTINATION bin
        )
    endif ()

    # 2. Install Headers
    # If INCLUDE_SUBDIR present then HEADERS put to include/SUBDIR, otherwise to include/
    set(DEST_INCLUDE_DIR "include")

    # Disable - it works bad
    # if (INSTALL_INCLUDE_SUBDIR)
    #     set(DEST_INCLUDE_DIR "include/${INSTALL_INCLUDE_SUBDIR}")
    # endif ()

    foreach (DIR IN LISTS INSTALL_INCLUDE_DIRS)
        install(DIRECTORY "${DIR}/"
                DESTINATION "${DEST_INCLUDE_DIR}"
                FILES_MATCHING
                PATTERN "*.h"
                PATTERN "*.hpp"
                PATTERN "*.inl"
                # Excludes:
                PATTERN ".git*" EXCLUDE
        )
    endforeach ()

    # 3. Generate Targets
    install(EXPORT ${INSTALL_TARGET}Targets
            FILE ${INSTALL_TARGET}Targets.cmake
            NAMESPACE ${INSTALL_NAMESPACE}::
            DESTINATION lib/cmake/${INSTALL_TARGET}
    )

    # 4. Create config file
    set(CONFIG_FILE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${INSTALL_TARGET}Config.cmake")

    file(WRITE "${CONFIG_FILE_PATH}"
            "include(CMakeFindDependencyMacro)\n"
            "${INSTALL_CONFIG_FILE_EXTRA_CONTENT}\n"
            "include(\"\${CMAKE_CURRENT_LIST_DIR}/${INSTALL_TARGET}Targets.cmake\")\n"
    )

    install(FILES "${CONFIG_FILE_PATH}"
            DESTINATION lib/cmake/${INSTALL_TARGET}
    )
endfunction()