if (NOT EMSCRIPTEN)
    message(FATAL_ERROR "This project should be build for EMSCRIPTEN only")
endif ()


add_executable(emscripten_basic_step02_minimalProject
        main.cpp
)

# Meed this line to fix error:
# error: Due to collision in variable name "Module", the shell file
set_target_properties(emscripten_basic_step02_minimalProject PROPERTIES SUFFIX ".js")

target_link_options(emscripten_basic_step02_minimalProject PRIVATE
        "-sMODULARIZE"
        "-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
)

add_custom_command(TARGET emscripten_basic_step02_minimalProject POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/index.html"
        "$<TARGET_FILE_DIR:emscripten_basic_step02_minimalProject>/index.html"
)