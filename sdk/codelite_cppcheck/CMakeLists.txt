
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles.
project(codelitegcc)

# Include paths
include_directories("${CL_SRC_ROOT}/sdk/codelite_cppcheck/lib" "${CL_SRC_ROOT}/sdk/codelite_cppcheck/cli")

set(ADDITIONAL_LIBRARIES "")

if(WIN32)
    set(ADDITIONAL_LIBRARIES "-lshlwapi")
endif(WIN32)

include_directories("${CL_SRC_ROOT}/sdk/codelite_cppcheck/externals/tinyxml")
include_directories("${CL_SRC_ROOT}/sdk/codelite_cppcheck/externals/simplecpp")
file(GLOB SRCS "cli/*.cpp" "lib/*.cpp" "externals/*/*.cpp")

# CppChecker 1.63 requires files stored in cfg/ at run-time.  For some daft reason, it only looks for this by default inside ${BINDIR/},
# which (at least for Linux) is _not_ the correct location. So we have to set an alternative place, and tell it at *compile-time*.
# AFAICT you can't just add an env var at run-time :/
if(UNIX AND NOT APPLE)
    add_definitions(-DCFGDIR=\"${CMAKE_INSTALL_PREFIX}/share/codelite/config/cfg\")
    if(CYGWIN)
        add_definitions(-D_GNU_SOURCE)
    endif(CYGWIN)
elseif(APPLE)
    add_definitions(-DCFGDIR=\"config/cppcheck\")
endif()

# Define the output
add_executable(codelite_cppcheck ${SRCS})
target_link_libraries(codelite_cppcheck ${LINKER_OPTIONS} ${ADDITIONAL_LIBRARIES})

cl_install_executable(codelite_cppcheck)

if(NOT APPLE)
    install(
        DIRECTORY ${CL_SRC_ROOT}/sdk/codelite_cppcheck/cfg
        DESTINATION ${CL_RESOURCES_DIR}/config
        USE_SOURCE_PERMISSIONS
        PATTERN ".svn" EXCLUDE
        PATTERN ".git" EXCLUDE)
endif()
