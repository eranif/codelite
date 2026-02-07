project("CodeliteTest")

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

file(GLOB UNIT_TESTS_SRC "*.cpp")
add_executable(CodeliteTest ${UNIT_TESTS_SRC})

target_link_libraries(
    CodeliteTest
    ${LINKER_OPTIONS}
    doctest
    libcodelite)

if(UNIX AND NOT APPLE)
    set_target_properties(CodeliteTest PROPERTIES
        SKIP_BUILD_RPATH FALSE
        BUILD_WITH_INSTALL_RPATH FALSE)
endif(UNIX AND NOT APPLE)
doctest_discover_tests(CodeliteTest)
