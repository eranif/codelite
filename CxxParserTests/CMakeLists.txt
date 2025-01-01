project(CxxLocalVariables)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(USE_PCH AND NOT MINGW)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif()

file(GLOB SRCS "*.cpp")

# Define the output
add_executable(CxxLocalVariables ${SRCS})

if(USE_PCH)
    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

# Remove the "lib" prefix from the plugin name
target_link_libraries(CxxLocalVariables ${LINKER_OPTIONS} libcodelite plugin wxsqlite3)
add_definitions(-DCXX_TEST_DIR=\"${CL_SRC_ROOT}/CxxParserTests/Test/\")
cl_install_executable(CxxLocalVariables)

add_test(NAME "CxxParserTests" COMMAND CxxLocalVariables)
