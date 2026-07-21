project(CxxLocalVariables)

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(USE_PCH AND NOT MINGW)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif()

file(GLOB SRCS "*.cpp")

# Embed the standard wxWidgets manifest on Windows so the test executable gets
# the same visual styles / DPI awareness setup as other wx-based tools.
set(CXXLOCALVARIABLES_RES_FILES "")
if(WIN32)
    set(CXXLOCALVARIABLES_RES_FILES "resources.rc")
endif()

# Define the output
add_executable(CxxLocalVariables ${SRCS} ${CXXLOCALVARIABLES_RES_FILES})

if(USE_PCH)
    target_precompile_headers(CxxLocalVariables REUSE_FROM PCH)
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(CxxLocalVariables PROPERTIES
        SKIP_BUILD_RPATH FALSE
        BUILD_WITH_INSTALL_RPATH FALSE)
endif(UNIX AND NOT APPLE)

target_link_libraries(CxxLocalVariables ${LINKER_OPTIONS} libcodelite plugin wxsqlite3 doctest)
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
  add_custom_command(TARGET CxxLocalVariables
    POST_BUILD
    COMMAND cmake -E copy_if_different ${WXDLL} $<TARGET_FILE_DIR:CxxLocalVariables>
    COMMAND_EXPAND_LISTS)

  add_custom_command(TARGET CxxLocalVariables
    POST_BUILD
    COMMAND cmake -E copy_if_different $<TARGET_RUNTIME_DLLS:CxxLocalVariables> $<TARGET_FILE_DIR:CxxLocalVariables>
    COMMAND_EXPAND_LISTS)
endif(MINGW)

add_compile_definitions(CXX_TEST_DIR=\"${CL_SRC_ROOT}/CxxParserTests/Test/\")
cl_install_executable(CxxLocalVariables)

doctest_discover_tests(CxxLocalVariables)
