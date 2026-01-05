cmake_minimum_required(VERSION 3.20)
project(3rd_party_libraries)

include(ExternalDependencies.cmake)

# Add paths to CMAKE_PREFIX_PATH so find_package can locate them
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/external_install/${CMAKE_BUILD_TYPE}")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/external_install/${CMAKE_BUILD_TYPE}/openssl/x64")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/external_install/${CMAKE_BUILD_TYPE}/OpenAL")

ExternalDependencies_print_global_variables()

ExternalDependencies_download_all()

add_subdirectory(examples)

ExternalDependencies_print_global_variables()