# define minimum cmake version
cmake_minimum_required(VERSION 3.0)
 
# Our project is called 'wxsqlite3' this is how it will be called in
# visual studio, and in our makefiles. 
project(wxsqlite3)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS core base REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# When building wxsqlite3, use MAKINGDLL 
if(WIN32)
    add_definitions(-DWXMAKINGDLL_WXSQLITE3)
endif(WIN32)

if ( USE_PCH )
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif ( USE_PCH )

include_directories(./include ${SQLITE3_INCLUDE_DIR})

if (UNIX AND NOT APPLE)
    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
endif()

if ( APPLE )
    add_definitions(-fPIC)
endif()


# For convenience we define the sources as a variable. You can add 
# header files and cpp/c files and CMake will sort them out
set(SRCS src/wxsqlite3.cpp)

# Define the output
add_library(wxsqlite3 SHARED ${SRCS})
target_link_libraries(wxsqlite3 ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} -L"${CL_LIBPATH}" ${SQLITE3_LIBRARY})

if (NOT MINGW)
    if(APPLE)
        install(TARGETS wxsqlite3 DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/)
        CL_INSTALL_NAME_TOOL_STD(${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/libwxsqlite3.dylib)
    else()
        install(TARGETS wxsqlite3 DESTINATION ${PLUGINS_DIR})
    endif()
else ()
    install(TARGETS wxsqlite3 RUNTIME DESTINATION ${CL_PREFIX}/bin LIBRARY DESTINATION ${CL_PREFIX}/lib ARCHIVE DESTINATION ${CL_PREFIX}/lib)
endif()
