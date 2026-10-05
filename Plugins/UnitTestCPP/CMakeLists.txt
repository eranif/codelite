# set the plugin name here
set(PLUGIN_NAME "UnitTestsPP")

# Our project is called 'plugin' this is how it will be called in visual studio, and in our makefiles.
project(${PLUGIN_NAME})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(USE_PCH AND NOT MINGW)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif()

wxcrafter_target(
    UnitTestCPP_NewClassTestBase
    "${CMAKE_CURRENT_SOURCE_DIR}/NewClassTestBase.wxcp"
    "testclassbasedlg.cpp"
    "testclassbasedlg.hpp")

wxcrafter_target(
    UnitTestCPP_NewUnitTestBase
    "${CMAKE_CURRENT_SOURCE_DIR}/newunittestbasedlg.wxcp"
    "newunittestbasedlg.cpp"
    "newunittestbasedlg.hpp")

wxcrafter_target(
    UnitTestCPP_unittestreport
    "${CMAKE_CURRENT_SOURCE_DIR}/unittestreport.wxcp"
    "unittestreport.cpp"
    "unittestreport.hpp")

file(GLOB SRCS "*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS}
    ${WXCRAFTER_UnitTestCPP_NewClassTestBase_OUTPUTS}
    ${WXCRAFTER_UnitTestCPP_NewUnitTestBase_OUTPUTS}
    ${WXCRAFTER_UnitTestCPP_unittestreport_OUTPUTS})

if(UNIX OR APPLE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

# Remove the "lib" prefix from the plugin name
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_include_directories(${PLUGIN_NAME} PUBLIC "${WXCRAFTER_UnitTestCPP_unittestreport_OUTPUTDIR}")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} libcodelite plugin)

cl_install_plugin(${PLUGIN_NAME})
