# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

file(GLOB_RECURSE SRCS "*.cpp")

wxcrafter_target(
  WxCrafterGui "${CMAKE_CURRENT_SOURCE_DIR}/UI/gui.wxcp"
  "${CMAKE_CURRENT_BINARY_DIR}" "gui.cpp" "gui.hpp" "gui_wxcrafter_bitmaps.cpp")

wxcrafter_target(
  WxCrafterWxCrafter "${CMAKE_CURRENT_SOURCE_DIR}/UI/wxcrafter.wxcp"
  "${CMAKE_CURRENT_BINARY_DIR}" "wxcrafter.cpp" "wxcrafter.hpp"
  "wxcrafter_wxcrafter_bitmaps.cpp")

wxcrafter_target(
  WxCrafterWxCrafterGui "${CMAKE_CURRENT_SOURCE_DIR}/UI/wxcrafter_gui.wxcp"
  "${CMAKE_CURRENT_BINARY_DIR}" "wxcrafter_gui.cpp" "wxcrafter_gui.hpp"
  "wxcrafter_gui_wxcrafter_bitmaps.cpp")

add_library(
  wxCrafterCommonGui STATIC
  ${SRCS} ${WXCRAFTER_WxCrafterGui_OUTPUTS}
  ${WXCRAFTER_WxCrafterWxCrafter_OUTPUTS}
  ${WXCRAFTER_WxCrafterWxCrafterGui_OUTPUTS})
set_target_properties(wxCrafterCommonGui PROPERTIES POSITION_INDEPENDENT_CODE
                                                    ON)

target_include_directories(wxCrafterCommonGui
                           PUBLIC
                           "${CMAKE_CURRENT_LIST_DIR}"
                           "${CMAKE_CURRENT_LIST_DIR}/Importer"
                           "${CMAKE_CURRENT_LIST_DIR}/UI"
                           "${CMAKE_CURRENT_LIST_DIR}/src"
                           "${CMAKE_CURRENT_LIST_DIR}/xrc_handlers"
                           "${WXCRAFTER_WxCrafterGui_OUTPUTDIR}")
target_link_libraries(wxCrafterCommonGui PUBLIC wxCrafterCommon plugin)
if(USE_PCH)
  target_precompile_headers(wxCrafterCommonGui REUSE_FROM PCH)
  if(NOT MINGW)
    target_compile_options(wxCrafterCommonGui PRIVATE -include "${CL_PCH_FILE}")
    target_compile_options(wxCrafterCommonGui PRIVATE -Winvalid-pch)
  endif()
endif()
