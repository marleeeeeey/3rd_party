if (NOT EMSCRIPTEN)
    message(FATAL_ERROR "This project should be build for EMSCRIPTEN only")
endif ()


add_executable(emscripten_basic_step04_minimalProject
        main.cpp
)

target_link_options(emscripten_basic_step04_minimalProject PRIVATE
        "--bind"
)

add_custom_command(TARGET emscripten_basic_step04_minimalProject POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/index.html"
        "$<TARGET_FILE_DIR:emscripten_basic_step04_minimalProject>/index.html"
)