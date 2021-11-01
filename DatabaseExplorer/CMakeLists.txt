


# set the plugin name here
set( PLUGIN_NAME "DatabaseExplorer")

# Our project is called 'plugin' this is how it will be called in
# visual studio, and in our makefiles.
project( ${PLUGIN_NAME} )

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.


# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

if (UNIX AND NOT APPLE)
    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
endif()

if ( APPLE )
    add_definitions(-fPIC)
endif()


if ( WITH_MYSQL )
    find_library(LIBMYSQLCLIENT NAMES libmysqlclient.so)
    if (  ${LIBMYSQLCLIENT} STREQUAL "LIBMYSQLCLIENT-NOTFOUND")
        message( FATAL_ERROR "-- Could not locate libmysqlclient.so" )
    else ( ${LIBMYSQLCLIENT} STREQUAL "LIBMYSQLCLIENT-NOTFOUND" )
        message( "-- LIBMYSQLCLIENT is set to ${LIBMYSQLCLIENT}" )
    endif ( ${LIBMYSQLCLIENT} STREQUAL "LIBMYSQLCLIENT-NOTFOUND" )
    add_definitions( -DDBL_USE_MYSQL=1 )
endif ( WITH_MYSQL )

## On UNIX we require GTK
if (UNIX AND NOT APPLE)

    if (GTK_VERSION EQUAL 3)
        set(OLD_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}) # Cache the current value
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
        find_package(GTK3)
        set(CMAKE_MODULE_PATH ${OLD_CMAKE_MODULE_PATH}) # Reset, else the official path isn't used again :/
        if (GTK3_FOUND)
            include_directories(${GTK3_INCLUDE_DIRS})
        else (GTK3_FOUND)
            message(FATAL_ERROR "Could not locate GTK.")
        endif (GTK3_FOUND)
    else()
        find_package(GTK2)
        if (GTK2_FOUND)
            include_directories(${GTK2_INCLUDE_DIRS})
        else (GTK2_FOUND)
            message(FATAL_ERROR "Could not locate GTK.")
        endif (GTK2_FOUND)
    endif()

endif (UNIX AND NOT APPLE)

if ( USE_PCH AND NOT MINGW )
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif()

# Include paths
include_directories("${CL_SRC_ROOT}/Plugin"
                    "${CL_SRC_ROOT}/sdk/wxsqlite3/include"
                    "${CL_SRC_ROOT}/CodeLite" "${CL_SRC_ROOT}/PCH"
                    "${CL_SRC_ROOT}/Interfaces"
                    "${CL_SRC_ROOT}/sdk/wxshapeframework/include"
                    "${CL_SRC_ROOT}/sdk/databaselayer/include" )

add_definitions(-DWXUSINGDLL_WXSQLITE3)
add_definitions(-DWXUSINGDLL_CL)
add_definitions(-DWXUSINGDLL_SDK)
add_definitions(-DDBL_USE_SQLITE)

FILE(GLOB SRCS "*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS})

target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)

# Remove the "lib" prefix from the plugin name
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")

if (GTK3_FOUND)
    set(GTK_LIBS ${GTK3_LIBRARIES})
else()
    set(GTK_LIBS ${GTK2_LIBRARIES})
endif()

target_link_libraries(${PLUGIN_NAME}
                      ${LINKER_OPTIONS}
                      ${wxWidgets_LIBRARIES}
                      ${GTK_LIBS}
                      libcodelite
                      plugin
                      wxshapeframework
                      databaselayersqlite
                      )
CL_INSTALL_PLUGIN(${PLUGIN_NAME})
