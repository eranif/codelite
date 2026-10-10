set(PLUGIN_NAME "codelitephp")
project(${PLUGIN_NAME})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

wxcrafter_target(
    PhpUi
    "${CMAKE_CURRENT_SOURCE_DIR}/php-plugin/php_ui.wxcp"
    "php_ui.cpp"
    "php_ui.hpp")

file(GLOB SRCS "php-plugin/*.cpp" "PHPParser/*.cpp")

add_library(${PLUGIN_NAME} SHARED ${SRCS} ${WXCRAFTER_PhpUi_OUTPUTS})

if(UNIX OR APPLE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# CodeLite plugins don't use the "lib" prefix
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_include_directories(
    ${PLUGIN_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/PHPParser" "${CMAKE_CURRENT_SOURCE_DIR}/php-plugin"
                           "${WXCRAFTER_PhpUi_OUTPUTDIR}")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} libcodelite plugin)
cl_install_plugin(${PLUGIN_NAME})
