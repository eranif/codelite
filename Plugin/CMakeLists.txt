# define minimum cmake version
cmake_minimum_required(VERSION 3.0)
 
# Our project is called 'plugin' this is how it will be called in
# visual studio, and in our makefiles. 
project(plugin)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS ${WX_COMPONENTS} REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# Include paths
include_directories("${CL_SRC_ROOT}/Plugin" "${CL_SRC_ROOT}/sdk/wxsqlite3/include" "${CL_SRC_ROOT}/CodeLite" "${CL_SRC_ROOT}/PCH" "${CL_SRC_ROOT}/Interfaces")

## Macros
if(WIN32)
    add_definitions(-DWXMAKINGDLL_SDK)
endif(WIN32)

if ( USE_PCH )
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif ( USE_PCH )


if (UNIX AND NOT APPLE)
    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
endif()

if ( APPLE )
    add_definitions(-fPIC)
endif()

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

# Additional libraries for UNIX-like systems
set( ADDITIONAL_LIBRARIES "" )

if (UNIX)
    if ( IS_FREEBSD )
        set(ADDITIONAL_LIBRARIES "-lkvm")
    elseif ( IS_NETBSD )
        set(ADDITIONAL_LIBRARIES "-lutil")
    elseif ( UNIX AND NOT APPLE )
        set(ADDITIONAL_LIBRARIES "-ldl -lutil")
    else ( )
        set(ADDITIONAL_LIBRARIES "-ldl")
    endif ( )
elseif (WIN32)
    set(ADDITIONAL_LIBRARIES "-luxtheme")
endif ( )

add_definitions(-DWXUSINGDLL_WXSQLITE3)
add_definitions(-DWXUSINGDLL_CL)

# Add RPATH
if(NOT MINGW AND WXC_APP)
    string( REPLACE "codelite" "wxcrafter" WXC_LIBS_DIR ${PLUGINS_DIR})
    set (LINKER_OPTIONS -Wl,-rpath,"${WXC_LIBS_DIR}")
    message( "-- libplugin.so is using RPATH set to ${WXC_LIBS_DIR}")
endif()

FILE(GLOB SRCS "*.cpp")

# Define the output
add_library(plugin SHARED ${SRCS})
if(GTK2_FOUND)
    target_link_libraries(plugin ${LINKER_OPTIONS} ${ADDITIONAL_LIBRARIES} ${GTK2_LIBRARIES} ${wxWidgets_LIBRARIES} -L"${CL_LIBPATH}" libcodelite)
elseif (GTK3_FOUND)
    target_link_libraries(plugin ${LINKER_OPTIONS} ${ADDITIONAL_LIBRARIES} ${GTK3_LIBRARIES} ${wxWidgets_LIBRARIES} -L"${CL_LIBPATH}" libcodelite)
else()
    target_link_libraries(plugin ${LINKER_OPTIONS} ${ADDITIONAL_LIBRARIES} ${wxWidgets_LIBRARIES} -L"${CL_LIBPATH}" libcodelite)
endif()

if (NOT MINGW)
    if(APPLE)
        install(TARGETS plugin DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/)
        CL_INSTALL_NAME_TOOL_STD(${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/libplugin.dylib)
    else()
        install(TARGETS plugin DESTINATION ${PLUGINS_DIR})
    endif()
else ()
    install(TARGETS plugin RUNTIME DESTINATION ${CL_PREFIX}/bin LIBRARY DESTINATION ${CL_PREFIX}/lib ARCHIVE DESTINATION ${CL_PREFIX}/lib)
endif()
CL_INSTALL_FILE_SHARED(${CL_SRC_ROOT}/Plugin/jsctags.zip)
