cmake_minimum_required(VERSION 3.16)
project(codelite-ctags)
if (UNIX AND NOT APPLE)
    set(CTAGS_ARCH_DIR "linux-x86_64")
endif()

if(APPLE)
    set(CTAGS_ARCH_DIR "macos")
endif()
# create a fake target out of
add_executable(codelite-ctags IMPORTED)
set_property(TARGET codelite-ctags PROPERTY
             IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/${CTAGS_ARCH_DIR}/codelite-ctags)
codelite_install_script(${CMAKE_CURRENT_LIST_DIR}/${CTAGS_ARCH_DIR}/codelite-ctags)
