# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(libcodelite)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS std REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# Include paths
include_directories("${CL_SRC_ROOT}/sdk/wxsqlite3/include" 
                    "${CL_SRC_ROOT}/sdk/codelite_indexer/network" 
                    "${CL_SRC_ROOT}/CodeLite" 
                    "${CL_SRC_ROOT}/PCH"
                    "${CL_SRC_ROOT}/Interfaces")

set( ADDITIONAL_LIBRARIES "" )

if (UNIX)
    if ( IS_FREEBSD )
        set(ADDITIONAL_LIBRARIES "-lkvm")
    elseif ( UNIX AND NOT APPLE )
        set(ADDITIONAL_LIBRARIES "-ldl -lutil")
    else ( )
        set(ADDITIONAL_LIBRARIES "-ldl")
    endif ( )
else (UNIX)
    set(ADDITIONAL_LIBRARIES "-lws2_32")
endif (UNIX)

## Macros
if(WIN32)
    add_definitions(-DWXMAKINGDLL_CL)
endif(WIN32)

if ( USE_PCH )
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif ( USE_PCH )

add_definitions(-DWXUSINGDLL_WXSQLITE3)

# Add RPATH
if (NOT MINGW)
if ( WXC_APP )
    string( REPLACE "codelite" "wxcrafter" WXC_LIBS_DIR ${PLUGINS_DIR}) 
    set (LINKER_OPTIONS -Wl,-rpath,"${WXC_LIBS_DIR}:${PLUGINS_DIR}")
    message( "-- libcodelite.so is using RPATH set to ${WXC_LIBS_DIR}:${PLUGINS_DIR}")
else ( WXC_APP )
    set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
    message( "-- libcodelite.so is using RPATH set to ${PLUGINS_DIR}")
endif ( WXC_APP )
endif (NOT MINGW)
FILE(GLOB SRCS "*.cpp" "../sdk/codelite_indexer/network/*.cpp" "SocketAPI/*.cpp")

# Define the output
add_library(libcodelite SHARED ${SRCS})
if (UNIX AND NOT APPLE )
    target_link_libraries(libcodelite ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} -L"${CL_LIBPATH}" -lsqlite3lib -lwxsqlite3 ${LIBSSH_LIB} ${ADDITIONAL_LIBRARIES})
else (UNIX AND NOT APPLE)
    target_link_libraries(libcodelite ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} -L"${CL_LIBPATH}" -lsqlite3lib -lwxsqlite3 ${LIBSSH_LIB} ${ADDITIONAL_LIBRARIES} -lz -lcrypto)
endif ( UNIX AND NOT APPLE )

if (NOT MINGW)
install(TARGETS libcodelite DESTINATION ${PLUGINS_DIR})
else ()
install(TARGETS libcodelite RUNTIME DESTINATION ${CL_PREFIX}/bin LIBRARY DESTINATION ${CL_PREFIX}/lib ARCHIVE DESTINATION ${CL_PREFIX}/lib)
endif()
