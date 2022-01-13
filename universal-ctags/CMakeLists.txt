cmake_minimum_required(VERSION 3.16)

# create a fake target out of
if (UNIX AND NOT APPLE)
    add_executable(uctags IMPORTED)
    set_property(TARGET uctags PROPERTY
                 IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/linux-x86_64/uctags)
    codelite_install_script(${CMAKE_CURRENT_LIST_DIR}/linux-x86_64/uctags)
endif()
