project("CodeliteTest")

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

file(GLOB_RECURSE UNIT_TESTS_SRC "*.cpp")
add_executable(CodeliteTest ${UNIT_TESTS_SRC})

target_link_libraries(CodeliteTest ${LINKER_OPTIONS} doctest libcodelite plugin)

if(MINGW)

  execute_process(
    COMMAND sh -c
            'ls ${WXWIN}/lib/clang_x64_dll/wxmsw*u_clang*custom.dll|tail -1'
    OUTPUT_VARIABLE WXDLL
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  if(NOT WXDLL)
    message(FATAL_ERROR "Could not locate wxWidgets DLL")
  endif()

  # Copy dlls near the executable
  add_custom_command(TARGET CodeliteTest
    POST_BUILD
    COMMAND cmake -E copy_if_different ${WXDLL} $<TARGET_FILE_DIR:CodeliteTest>
    COMMAND_EXPAND_LISTS)

  add_custom_command(TARGET CodeliteTest
    POST_BUILD
    COMMAND cmake -E copy_if_different $<TARGET_RUNTIME_DLLS:CodeliteTest> $<TARGET_FILE_DIR:CodeliteTest>
    COMMAND_EXPAND_LISTS)
endif(MINGW)

if(UNIX AND NOT APPLE)
  set_target_properties(CodeliteTest PROPERTIES SKIP_BUILD_RPATH FALSE
                                                BUILD_WITH_INSTALL_RPATH FALSE)
endif(UNIX AND NOT APPLE)
doctest_discover_tests(CodeliteTest)
