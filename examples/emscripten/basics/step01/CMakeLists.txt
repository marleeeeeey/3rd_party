if (NOT EMSCRIPTEN)
    message(FATAL_ERROR "This project should be build for EMSCRIPTEN only")
endif ()


add_executable(emscripten_basic_step01_minimalProject
        main.cpp
)

target_link_options(emscripten_basic_step01_minimalProject PRIVATE
        "-sEXPORTED_FUNCTIONS=['_int_sqrt']"
        "-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
)