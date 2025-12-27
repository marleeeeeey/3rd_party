include("../../../FlatbuffersHelper.cmake")

add_flatbuffers_schema(solder_schema
        solder.fbs
)

add_subdirectory(client)
add_subdirectory(server)