# set the plugin name here
set(PLUGIN_NAME "DatabaseExplorer")

# Our project is called 'plugin' this is how it will be called in visual studio, and in our makefiles.
project(${PLUGIN_NAME})

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(WITH_MYSQL)
    find_library(LIBMYSQLCLIENT NAMES libmysqlclient.so)
    if(${LIBMYSQLCLIENT} STREQUAL "LIBMYSQLCLIENT-NOTFOUND")
        message(FATAL_ERROR "-- Could not locate libmysqlclient.so")
    else(${LIBMYSQLCLIENT} STREQUAL "LIBMYSQLCLIENT-NOTFOUND")
        message("-- LIBMYSQLCLIENT is set to ${LIBMYSQLCLIENT}")
    endif(${LIBMYSQLCLIENT} STREQUAL "LIBMYSQLCLIENT-NOTFOUND")
    add_definitions(-DDBL_USE_MYSQL=1)

    if(MINGW)
        install(FILES "${MSYS2_BASE}/clang64/bin/libmariadb.dll" DESTINATION "${CMAKE_INSTALL_PREFIX}")
        msys_list_deps(${MSYS2_BASE}/clang64/bin/libmariadb.dll LIBMARIA_DEPS)
        foreach(DEP ${LIBMARIA_DEPS})
            install(FILES "${DEP}" DESTINATION "${CMAKE_INSTALL_PREFIX}")
        endforeach()
    endif()
    unset(WITH_MYSQL CACHE)
endif(WITH_MYSQL)

if(WITH_POSTGRES)
    find_package(PostgreSQL REQUIRED)

    add_compile_definitions(DBL_USE_POSTGRES=1)

    if(MINGW)
        install(FILES "${MSYS2_BASE}/clang64/bin/libpq.dll" DESTINATION "${CMAKE_INSTALL_PREFIX}")
        msys_list_deps(${MSYS2_BASE}/clang64/bin/libpq.dll LIBPOSTGRES_DEPS)
        foreach(DEP ${LIBPOSTGRES_DEPS})
            install(FILES "${DEP}" DESTINATION "${CMAKE_INSTALL_PREFIX}")
        endforeach()
    endif()
endif()

# On UNIX we require GTK
if(UNIX AND NOT APPLE)

    if(GTK_VERSION EQUAL 3)
        set(OLD_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}) # Cache the current value
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
        find_package(GTK3)
        set(CMAKE_MODULE_PATH ${OLD_CMAKE_MODULE_PATH}) # Reset, else the official path isn't used again :/
        if(GTK3_FOUND)
            include_directories(${GTK3_INCLUDE_DIRS})
        else(GTK3_FOUND)
            message(FATAL_ERROR "Could not locate GTK.")
        endif(GTK3_FOUND)
    else()
        find_package(GTK2)
        if(GTK2_FOUND)
            include_directories(${GTK2_INCLUDE_DIRS})
        else(GTK2_FOUND)
            message(FATAL_ERROR "Could not locate GTK.")
        endif(GTK2_FOUND)
    endif()

endif(UNIX AND NOT APPLE)

if(USE_PCH AND NOT MINGW)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif()

# Include paths
include_directories(
    "${CL_SRC_ROOT}/sdk/databaselayer/include")

add_definitions(-DDBL_USE_SQLITE)

file(GLOB SRCS "*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS})

if(UNIX OR APPLE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

# Remove the "lib" prefix from the plugin name
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")

if(GTK3_FOUND)
    set(GTK_LIBS ${GTK3_LIBRARIES})
else()
    set(GTK_LIBS ${GTK2_LIBRARIES})
endif()

target_link_libraries(
    ${PLUGIN_NAME}
    ${LINKER_OPTIONS}
    ${wxWidgets_LIBRARIES}
    ${GTK_LIBS}
    libcodelite
    plugin
    wxshapeframework
    databaselayersqlite)
cl_install_plugin(${PLUGIN_NAME})
