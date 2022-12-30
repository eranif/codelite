# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles.
project(libcodelite)

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(UNIX AND NOT APPLE)
    set(CMAKE_SKIP_BUILD_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    list(PREPEND CMAKE_INSTALL_RPATH "${PLUGINS_DIR}")
endif(UNIX AND NOT APPLE)

# Flex files
if(UNIX AND NOT APPLE)
    if(WITH_FLEX)
        message("-- Adding flex support")
        find_package(FLEX REQUIRED)

        set(CxxFlexOutput ${CMAKE_SOURCE_DIR}/CodeLite/CxxLexer.cpp)
        set(CxxFlexInput ${CMAKE_SOURCE_DIR}/CodeLite/CxxScanner.l)

        set(PhpFlexOutput ${CMAKE_SOURCE_DIR}/CodeLite/PhpLexer.cpp)
        set(PhpFlexInput ${CMAKE_SOURCE_DIR}/CodeLite/PhpLexer.l)

        set(JSFlexOutput ${CMAKE_SOURCE_DIR}/CodeLite/JSLexer.cpp)
        set(JSFlexInput ${CMAKE_SOURCE_DIR}/CodeLite/JSLexer.l)

        if(FLEX_FOUND)
            add_custom_command(
                OUTPUT ${CxxFlexOutput}
                COMMAND ${FLEX_EXECUTABLE} --noline --yylineno --batch --outfile=${CxxFlexOutput} ${CxxFlexInput}
                DEPENDS ${CxxFlexInput}
                COMMENT "Generating ${CxxFlexOutput}")
            add_custom_command(
                OUTPUT ${PhpFlexOutput}
                COMMAND ${FLEX_EXECUTABLE} -Pphp --yylineno --noline --batch --outfile=${PhpFlexOutput} ${PhpFlexInput}
                DEPENDS ${PhpFlexInput}
                COMMENT "Generating ${PhpFlexOutput}")
            add_custom_command(
                OUTPUT ${JSFlexOutput}
                COMMAND ${FLEX_EXECUTABLE} -Pjs --yylineno --noline --batch --outfile=${JSFlexOutput} ${JSFlexInput}
                DEPENDS ${JSFlexInput}
                COMMENT "Generating ${JSFlexOutput}")
        endif()
    else()
        message("-- Flex support is disabled")
    endif()
endif()

# Include paths
include_directories(
    "${CL_SRC_ROOT}/sdk/wxsqlite3/include"
    "${CL_SRC_ROOT}/sdk/codelite_indexer/network"
    "${CL_SRC_ROOT}/sdk/websocketpp"
    "${CL_SRC_ROOT}/sdk/asio-1.12.1"
    "${CL_SRC_ROOT}/CodeLite"
    "${CL_SRC_ROOT}/PCH"
    "${CL_SRC_ROOT}/Interfaces")

set(ADDITIONAL_LIBRARIES "")

if(UNIX)
    if(IS_FREEBSD)
        set(ADDITIONAL_LIBRARIES "-lkvm")
    elseif(IS_NETBSD)
        set(ADDTIONAL_LIBRARIES "-lutil")
    elseif(UNIX AND NOT APPLE)
        set(ADDITIONAL_LIBRARIES "-ldl -lutil")
    else()
        set(ADDITIONAL_LIBRARIES "-ldl -lz")
    endif()
else(UNIX)
    set(ADDITIONAL_LIBRARIES "-lws2_32")
endif(UNIX)

## Macros
if(WIN32)
    add_definitions(-DWXMAKINGDLL_CL)
endif(WIN32)
add_definitions(-DwxUSE_GUI=1)

if(USE_PCH AND NOT MINGW)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif()

if(UNIX AND NOT APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

if(APPLE)
    add_definitions(-fPIC)
endif()

add_definitions(-DWXUSINGDLL_WXSQLITE3)

# Add RPATH
if(NOT MINGW AND WXC_APP)
    string(REPLACE "codelite" "wxcrafter" WXC_LIBS_DIR ${PLUGINS_DIR})
    set(LINKER_OPTIONS -Wl,-rpath,"${WXC_LIBS_DIR}")
    message("-- libcodelite.so is using RPATH set to ${WXC_LIBS_DIR}")
endif()
file(GLOB SRCS "*.cpp" "../sdk/codelite_indexer/network/*.cpp" "SocketAPI/*.cpp" "LSP/*.cpp")

# Define GTK libraries
if(GTK2_FOUND)
    set(GTK_LIBS "${GTK2_LIBRARIES}")
elseif(GTK3_FOUND)
    set(GTK_LIBS "${GTK3_LIBRARIES}")
else()
    set(GTK_LIBS "")
endif()

# Define the output
add_library(libcodelite SHARED ${SRCS})
target_link_libraries(
    libcodelite
    ${LINKER_OPTIONS}
    ${wxWidgets_LIBRARIES}
    -L"${CL_LIBPATH}"
    ${SQLite3_LIBRARIES}
    wxsqlite3
    ${LIBSSH_LIB}
    ${GTK_LIBS}
    ${ADDITIONAL_LIBRARIES}
    ${LIBUCHARDET_LIB})

if(MINGW)
    set_target_properties(
        libcodelite
        PROPERTIES PREFIX ""
                   OUTPUT_NAME libcodelite
                   RUNTIME_OUTPUT_NAME codelite
                   ARCHIVE_OUTPUT_NAME codelite)
endif()

codelite_add_pch(libcodelite)

if(NOT MINGW)
    if(APPLE)
        install(TARGETS libcodelite DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/)
        cl_install_name_tool_std(${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/liblibcodelite.dylib)
        cl_osx_copy_brew_lib(${LIBSSH_LIB} ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS)
    else()
        install(TARGETS libcodelite DESTINATION ${PLUGINS_DIR})
    endif()
else()
    install(
        TARGETS libcodelite
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif()
