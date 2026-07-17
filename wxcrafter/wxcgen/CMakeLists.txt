# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

# Add RPATH When building an application, set the install library to the proper
# location which is now under /usr/lib/x86_64-linux-gnu/wxcrafter
if(NOT MINGW)
  if(WXC_APP)
    set(WXC_INSTALL_LIBDIR "lib")
    if(NOT PREFIX)
      # If the caller hasn't set his own destination, install to a multi-arch
      # lib dir if applicable
      if(CMAKE_VERSION VERSION_GREATER 2.8.7 AND (UNIX AND NOT APPLE))
        include(GNUInstallDirs)
        # defines CMAKE_INSTALL_LIBDIR to lib or lib64 or whatever. Available
        # since cmake 2.8.8
        set(WXC_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
      endif()
    endif(NOT PREFIX)
    set(WXC_LIBS_DIR
        "${CMAKE_INSTALL_PREFIX}/${WXC_INSTALL_LIBDIR}/${APP_NAME}")
    set(LINKER_OPTIONS -Wl,-rpath,"${WXC_LIBS_DIR}")
  else(WXC_APP)
    if(NOT OFFICIAL_CL_BUILD)
      set(LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
    endif(NOT OFFICIAL_CL_BUILD)
  endif(WXC_APP)
endif()

set(RES_FILES "")
if(MINGW)
  set(RES_FILES "../resources/wxcgen_resources.rc")
endif()

# Headless CLI code-generator (no DISPLAY required).
file(GLOB_RECURSE SRCS "*.cpp")
add_executable(wxcgen ${SRCS} ${RES_FILES})
target_link_libraries(wxcgen PRIVATE wxCrafterCommon)
target_link_options(wxcgen PRIVATE ${LINKER_OPTIONS} $<IF:$<BOOL:${MINGW}>,-mconsole,-lutil>)

if(USE_PCH AND NOT MINGW)
  target_compile_options(wxcgen PRIVATE -include "${CL_PCH_FILE}")
  target_compile_options(wxcgen PRIVATE -Winvalid-pch)
endif()

target_link_libraries(wxcgen PRIVATE ${LINKER_OPTIONS}
                                     $<IF:$<BOOL:${MINGW}>,-mconsole,-lutil>)

if(UNIX AND NOT APPLE)
  set_target_properties(wxcgen PROPERTIES SKIP_BUILD_RPATH FALSE
                                          BUILD_WITH_INSTALL_RPATH FALSE)
endif(UNIX AND NOT APPLE)

# Copy Dlls near executable
if(MINGW)
  execute_process(
    COMMAND sh -c
            'ls ${WXWIN}/lib/clang_x64_dll/wxmsw*u_clang*custom.dll|tail -1'
    OUTPUT_VARIABLE WXDLL
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  if(NOT WXDLL)
    message(FATAL_ERROR "Could not locate wxWidgets DLL")
  endif()

  # Get DLL dependencies for additional libraries needed by wxcgen
  msys_list_deps(${MSYS2_BASE}/${MSYSTEM}/bin/libssh.dll LIBSSH_DLLS)
  msys_list_deps(${MSYS2_BASE}/${MSYSTEM}/bin/libc++.dll LIBCXX_DLLS)
  msys_list_deps(${MSYS2_BASE}/${MSYSTEM}/bin/openssl.exe OPENSSL_DLLS)

  # Combine all additional DLLs
  list(APPEND WXCGEN_EXTRA_DLLS ${LIBSSH_DLLS} ${LIBCXX_DLLS} ${OPENSSL_DLLS})
  list(APPEND WXCGEN_EXTRA_DLLS "${MSYS2_BASE}/${MSYSTEM}/bin/libsqlite3-0.dll")
  list(REMOVE_DUPLICATES WXCGEN_EXTRA_DLLS)

  # Copy dlls near the wxcgen executable
  add_custom_command(
    TARGET wxcgen
    POST_BUILD
    COMMAND cmake -E copy_if_different ${WXDLL} $<TARGET_FILE_DIR:wxcgen>
    COMMAND_EXPAND_LISTS)
  add_custom_command(
    TARGET wxcgen
    POST_BUILD
    COMMAND cmake -E copy_if_different $<TARGET_RUNTIME_DLLS:wxcgen>
            $<TARGET_FILE_DIR:wxcgen>
    COMMAND_EXPAND_LISTS)

  # Copy additional dependencies for wxcgen
  foreach(DLL ${WXCGEN_EXTRA_DLLS})
    add_custom_command(
      TARGET wxcgen
      POST_BUILD
      COMMAND cmake -E copy_if_different "${DLL}" $<TARGET_FILE_DIR:wxcgen>
      COMMAND_EXPAND_LISTS)
  endforeach()
endif(MINGW)

# MACRO

macro(
  wxcrafter_target
  Name
  Input
  OutputDir
  OutputCpp
  OutputHpp
  OutputBitmaps
  # Extra output for models
)

  set(WXCRAFTER_${Name}_OUTPUTS
      "${OutputDir}/${OutputCpp}" "${OutputDir}/${OutputHpp}"
      "${OutputDir}/${OutputBitmaps}")

  set(ExtraMacroArgs ${ARGN}) # ARGN is not a variable
  foreach(ExtraArgs ${ExtraMacroArgs})
    list(APPEND WXCRAFTER_${Name}_OUTPUTS "${OutputDir}/${ExtraArgs}")
  endforeach()
  unset(ExtraMacroArgs)

  set(WXCGEN_CONFIG_FILE "${CMAKE_SOURCE_DIR}/.wxcgen.json")
  add_custom_command(
    OUTPUT ${WXCRAFTER_${Name}_OUTPUTS}
    COMMAND $<TARGET_FILE:wxcgen> -o "${OutputDir}" -p "${WXCGEN_CONFIG_FILE}"
            "${Input}"
    VERBATIM
    DEPENDS ${Input} wxcgen
    COMMENT "[WxCrafter][${Name}] Building code (wxcgen)")

  set(WXCRAFTER_${Name}_OUTPUTDIR "${OutputDir}")
endmacro()

# Install

if(MINGW)
  install(TARGETS wxcgen DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/")
else(MINGW)
  set(CPACK_DEB_COMPONENT_INSTALL 1)

  # Installing the applications
  install(
    TARGETS wxcgen
    DESTINATION ${CL_INSTALL_BIN}
    PERMISSIONS ${EXE_PERM}
    COMPONENT standalone)
endif(MINGW)
