find_package(Gettext)

if (GETTEXT_FOUND)

    set(INSTALL_LANG_DIR ${CMAKE_INSTALL_PREFIX}/share/locale)

# 2 issues with GETTEXT_PROCESS_PO_FILES:
# - gitlab.kitware.com/cmake/cmake/-/issues/14904: Gettext module does not allow installing .po files with the same name from different directories
# - gitlab.kitware.com/cmake/cmake/-/issues/20792: FindGettext: gettext_process_po_files: Include a RENAME parameter
function(GETTEXT_PROCESS_PO_FILES_FIXED _lang)
  set(_options ALL)
  set(_oneValueArgs INSTALL_DESTINATION)
  set(_multiValueArgs PO_FILES)
  set(_gmoFiles)

  CMAKE_PARSE_ARGUMENTS(_parsedArguments "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN})

  foreach(_current_PO_FILE ${_parsedArguments_PO_FILES})
    get_filename_component(_name ${_current_PO_FILE} NAME)
    string(REGEX REPLACE "^(.+)(\\.[^.]+)$" "\\1" _basename ${_name})
    set(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}-${_basename}.gmo)
    add_custom_command(OUTPUT ${_gmoFile}
      COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_current_PO_FILE}
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      DEPENDS ${_current_PO_FILE}
    )

    if(_parsedArguments_INSTALL_DESTINATION)
      install(FILES ${_gmoFile} DESTINATION ${_parsedArguments_INSTALL_DESTINATION}/${_lang}/LC_MESSAGES/ RENAME ${_basename}.mo)
    endif()
    list(APPEND _gmoFiles ${_gmoFile})
  endforeach()


  if(NOT TARGET pofiles)
    add_custom_target(pofiles)
  endif()

  _GETTEXT_GET_UNIQUE_TARGET_NAME( pofiles uniqueTargetName)

  if(_parsedArguments_ALL)
    add_custom_target(${uniqueTargetName} ALL DEPENDS ${_gmoFiles})
  else()
    add_custom_target(${uniqueTargetName} DEPENDS ${_gmoFiles})
  endif()

  add_dependencies(pofiles ${uniqueTargetName})

endfunction()

    foreach(LANG_DIR cs fr it ja_JP ru_RU zh_CN)
        GETTEXT_PROCESS_PO_FILES_FIXED(${LANG_DIR} ALL INSTALL_DESTINATION ${INSTALL_LANG_DIR} PO_FILES ${LANG_DIR}/codelite.po)
        if(MINGW)
            FILE(GLOB WX_WIDGETS_MO "${WXWIN}/share/locale/${LANG_DIR}/LC_MESSAGES/wxstd-*.*.*mo")
            install(FILES ${WX_WIDGETS_MO} DESTINATION ${INSTALL_LANG_DIR}/${LANG_DIR}/LC_MESSAGES/)
        endif()
    endforeach()

else()
    MESSAGE(WARNING "gettext not found")
endif()
