# build the Core part of CodeLite (regardless of the final target)
if(APPLE)
    function(build_and_install_submodule SRC_DIR BUILD_DIR CMAKE_ARGS)
        execute_process(COMMAND mkdir -p ${BUILD_DIR} COMMAND_ERROR_IS_FATAL ANY)
        execute_process(
            COMMAND ${CMAKE_COMMAND} ${SRC_DIR} -DCMAKE_BUILD_TYPE=Release
                    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/local_builds "${CMAKE_ARGS}"
            WORKING_DIRECTORY ${BUILD_DIR} COMMAND_ERROR_IS_FATAL ANY)
        execute_process(COMMAND ${CMAKE_COMMAND} --build . --parallel --target install
                        WORKING_DIRECTORY "${BUILD_DIR}" COMMAND_ERROR_IS_FATAL ANY)
    endfunction()

    set(SUBMODULES_DIR ${CMAKE_SOURCE_DIR}/submodules)

    # Skip the build if ALL the targets are already built
    if(EXISTS "${CMAKE_BINARY_DIR}/local_builds/lib/libssh.a"
       AND EXISTS "${CMAKE_BINARY_DIR}/local_builds/lib/libhunspell-1.7.a"
       AND EXISTS "${CMAKE_BINARY_DIR}/local_builds/lib/libssl.a"
       AND EXISTS "${CMAKE_BINARY_DIR}/local_builds/lib/libz.a"
       AND EXISTS "${CMAKE_BINARY_DIR}/local_builds/lib/libcrypto.a")
        message(STATUS "LibSSH is already built, skipping")
        set(LIBSSH_INCLUDE_DIR "${CMAKE_BINARY_DIR}/local_builds/include")
        set(LIBSSH_LIB
            "${CMAKE_BINARY_DIR}/local_builds/lib/libssh.a;${CMAKE_BINARY_DIR}/local_builds/lib/libcrypto.a;${CMAKE_BINARY_DIR}/local_builds/lib/libssl.a;${CMAKE_BINARY_DIR}/local_builds/lib/libz.a"
        )
        set(LIBYAML_CPP "${CMAKE_BINARY_DIR}/local_builds/lib/libyaml-cpp.a")
        set(LIBHUNSPELL "${CMAKE_BINARY_DIR}/local_builds/lib/libhunspell-1.7.a")
        set(HUNSPELL_H "${CMAKE_BINARY_DIR}/local_builds/include")
    else()
        # Under macOS, we compile OpenSSL, libSSH, Zlib, HunSpell + YamlCPP from sources

        # Configure, build & install OpenSSL
        message(STATUS "Building OpenSSL...")
        build_and_install_submodule("${SUBMODULES_DIR}/openssl-cmake" "${CMAKE_BINARY_DIR}/openssl-cmake-build" "")

        # Configure, build and & install libssh
        message(STATUS "Building libssh...")
        execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/libssh-build COMMAND_ERROR_IS_FATAL ANY)
        execute_process(
            COMMAND
                ${CMAKE_COMMAND} ${SUBMODULES_DIR}/libssh -DCMAKE_BUILD_TYPE=Release
                -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/local_builds
                -DOPENSSL_ROOT_DIR=${CMAKE_BINARY_DIR}/local_builds -DWITH_EXAMPLES=OFF -DBUILD_SHARED_LIBS=OFF
                -DWITH_SERVER=OFF -DWITH_GSSAPI=OFF -DWITH_EXAMPLES=OFF
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libssh-build COMMAND_ERROR_IS_FATAL ANY)
        execute_process(COMMAND ${CMAKE_COMMAND} --build . --parallel --target install
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libssh-build COMMAND_ERROR_IS_FATAL ANY)

        # Configure, build and & install libz
        message(STATUS "Building zlib...")
        build_and_install_submodule("${SUBMODULES_DIR}/zlib" "${CMAKE_BINARY_DIR}/zlib-build" "")

        # Configure, build and & install yaml-cpp
        message(STATUS "Building yaml-cpp...")
        build_and_install_submodule("${CMAKE_SOURCE_DIR}/submodules/yaml-cpp" "${CMAKE_BINARY_DIR}/yaml-cpp-build"
                                    "-DBUILD_SHARED_LIBS=OFF")

        # Configure & build HunSpell (no CMake)
        message(STATUS "Building HunSpell...")
        set(HUNSPELL_BUILD_DIR ${CMAKE_BINARY_DIR}/hunspell-build)
        set(HUNSPELL_SRC_DIR ${SUBMODULES_DIR}/hunspell)
        execute_process(COMMAND mkdir -p "${HUNSPELL_BUILD_DIR}" COMMAND_ERROR_IS_FATAL ANY)
        execute_process(COMMAND autoreconf -vfi WORKING_DIRECTORY "${HUNSPELL_SRC_DIR}" COMMAND_ERROR_IS_FATAL ANY)
        execute_process(
            COMMAND ${HUNSPELL_SRC_DIR}/configure --enable-shared=no --enable-static=yes
                    --prefix=${CMAKE_BINARY_DIR}/local_builds WORKING_DIRECTORY "${HUNSPELL_BUILD_DIR}"
                                                                                COMMAND_ERROR_IS_FATAL ANY)
        execute_process(COMMAND make -j4 install WORKING_DIRECTORY "${HUNSPELL_BUILD_DIR}" COMMAND_ERROR_IS_FATAL ANY)

        # Define variables to be used by the project
        set(LIBSSH_LIB
            "${CMAKE_BINARY_DIR}/local_builds/lib/libssh.a;${CMAKE_BINARY_DIR}/local_builds/lib/libcrypto.a;${CMAKE_BINARY_DIR}/local_builds/lib/libssl.a;${CMAKE_BINARY_DIR}/local_builds/lib/libz.a"
        )
        set(LIBYAML_CPP "${CMAKE_BINARY_DIR}/local_builds/lib/libyaml-cpp.a")
        set(LIBHUNSPELL "${CMAKE_BINARY_DIR}/local_builds/lib/libhunspell-1.7.a")
        set(HUNSPELL_H "${CMAKE_BINARY_DIR}/local_builds/include")
    endif()
    include_directories(${CMAKE_BINARY_DIR}/local_builds/include)
endif()
