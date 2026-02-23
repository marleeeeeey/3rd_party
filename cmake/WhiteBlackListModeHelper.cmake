include_guard(GLOBAL)

# ---------------------------------------------------
# Whitelist mode: build only listed libs (exact match)
# Example:
#   -D3RD_PARTY_ONLY="imgui;SDL3;glm;asio;ixwebsocket"
# Empty = build all
#
# or
#
# set(3RD_PARTY_ONLY "imgui;SDL3" CACHE STRING "" FORCE)
# add_subdirectory(3rd_party)
# ---------------------------------------------------
set(3RD_PARTY_ONLY "" CACHE STRING "Semicolon-separated whitelist of libs to build (exact match). Empty = build all.")

function(white_list_filter inListVar outListVar)
    if (3RD_PARTY_ONLY STREQUAL "")
        set(${outListVar} "${${inListVar}}" PARENT_SCOPE)
        return()
    endif ()

    set(result "")
    foreach (lib IN LISTS ${inListVar})
        list(FIND 3RD_PARTY_ONLY "${lib}" idx)
        if (NOT idx EQUAL -1)
            list(APPEND result "${lib}")
        endif ()
    endforeach ()

    set(${outListVar} "${result}" PARENT_SCOPE)
endfunction()

# Blacklist mode: skip listed libs (exact match)
# Example:
#   -D3RD_PARTY_SKIP="tracy;flatbuffers;openal-soft;uWebSockets"
# Empty = skip nothing
#
# or
#
# set(3RD_PARTY_SKIP "tracy;flatbuffers;openal-soft" CACHE STRING "" FORCE)
# add_subdirectory(3rd_party)
# ---------------------------------------------------
set(3RD_PARTY_SKIP "" CACHE STRING "Semicolon-separated blacklist of libs to skip (exact match). Empty = skip nothing.")

function(black_list_filter inListVar outListVar)
    if (3RD_PARTY_SKIP STREQUAL "")
        set(${outListVar} "${${inListVar}}" PARENT_SCOPE)
        return()
    endif ()

    set(result "")
    foreach (lib IN LISTS ${inListVar})
        list(FIND 3RD_PARTY_SKIP "${lib}" idx)
        if (idx EQUAL -1)
            list(APPEND result "${lib}")
        endif ()
    endforeach ()

    set(${outListVar} "${result}" PARENT_SCOPE)
endfunction()