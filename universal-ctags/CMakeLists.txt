cmake_minimum_required(VERSION 3.16)
project(codelite-ctags)

# create a fake target out of
if (UNIX AND NOT APPLE)
    add_executable(codelite-ctags IMPORTED)
    set_property(TARGET codelite-ctags PROPERTY
                 IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/linux-x86_64/codelite-ctags)
    codelite_install_script(${CMAKE_CURRENT_LIST_DIR}/linux-x86_64/codelite-ctags)
endif()
