cmake_minimum_required(VERSION 3.20)
project(3rd_party_libraries)

include(download_all.cmake)
run_all_downloads()

add_subdirectory(examples)