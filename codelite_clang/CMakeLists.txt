# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(codelite-clang)

find_package(wxWidgets COMPONENTS base REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# Include paths
include_directories("${CLANG_INCLUDE}")

# Add RPATH
if (UNIX)
set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
endif (UNIX)

FILE(GLOB SRCS "*.cpp")

# Define the output
add_executable(codelite-clang ${SRCS})
target_link_libraries(codelite-clang ${wxWidgets_LIBRARIES} ${CLANG_LIBRARY})

install(TARGETS codelite-clang DESTINATION ${CL_PREFIX}/bin)

