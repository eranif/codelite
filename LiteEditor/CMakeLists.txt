# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(codelite)

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CL_ROOT}/lib" )

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS std aui propgrid stc REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# Include paths
include_directories("${CL_ROOT}/Plugin" "${CL_ROOT}/sdk/wxsqlite3/include" "${CL_ROOT}/CodeLite" "${CL_ROOT}/PCH" "${CL_ROOT}/Interfaces")

## On UNIX we require GTK
if (UNIX AND NOT APPLE)

    find_package(GTK2)
    if (GTK2_FOUND)
        message("GTK2 Found... yes")
        include_directories("${GTK2_INCLUDE_DIRS}")
    else (GTK2_FOUND)
        message(FATAL_ERROR "Could not locate GTK.")
    endif (GTK2_FOUND)

endif (UNIX AND NOT APPLE)

add_definitions(-DWXUSINGDLL_WXSQLITE3)
add_definitions(-DWXUSINGDLL_CL)
add_definitions(-DWXUSINGDLL_SDK)

# Add RPATH
set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")

FILE(GLOB SRCS "*.cpp")

# Define the output
add_executable(codelite ${SRCS})
if(GTK2_FOUND)
    target_link_libraries(codelite ${LINKER_OPTIONS} ${GTK2_LIBRARIES} ${wxWidgets_LIBRARIES} -L${CL_ROOT}/lib -lwxsqlite3 -lsqlite3 -llibcodelite -lplugin -lutil)
else (GTK2_FOUND)
    target_link_libraries(codelite ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} -L${CL_ROOT}/lib -lwxsqlite3 -lsqlite3 -llibcodelite -lplugin -lutil)
endif(GTK2_FOUND)

install(TARGETS codelite DESTINATION ${PREFIX}/bin)

