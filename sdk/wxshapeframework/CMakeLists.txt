# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'wxshapeframework' this is how it will be called in
# visual studio, and in our makefiles. 
project(wxshapeframework)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS std REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

if(WIN32)
    add_definitions(-DWXMAKINGDLL_WXXS)
    add_definitions(-DWXMAKINGDLL_WXSF)
endif(WIN32)

if ( USE_PCH )
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif ( USE_PCH )


include_directories("${CL_SRC_ROOT}/sdk/wxshapeframework/include" "${CL_SRC_ROOT}/sdk/wxshapeframework/src")

# Add RPATH
if (UNIX)
set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
endif (UNIX)

FILE(GLOB SRCS "src/*.cpp" "src/wxxmlserializer/*.cpp")

# Define the output
add_library(wxshapeframework SHARED ${SRCS})
target_link_libraries(wxshapeframework ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES})

if (NOT MINGW)
install(TARGETS wxshapeframework DESTINATION ${PLUGINS_DIR})
else ()
install(TARGETS wxshapeframework RUNTIME DESTINATION ${CL_PREFIX}/bin LIBRARY DESTINATION ${CL_PREFIX}/lib ARCHIVE DESTINATION ${CL_PREFIX}/lib)
endif()
