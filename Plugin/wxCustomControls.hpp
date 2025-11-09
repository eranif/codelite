#ifndef WXCUSTOMCONTROLS_HPP
#define WXCUSTOMCONTROLS_HPP

#ifdef __WXGTK__
#include "wxCustomControlsGTK.hpp"
#elif defined(__WXOSX__)
// macOS
#include "wxCustomControlsMacOS.hpp"
#else
// Windows
#include "wxCustomControlsMSW.hpp"
#endif

// clang-format off
#include "codelite_exports.h"
#include <wx/app.h>
#include <wx/window.h>
#include <wx/string.h>
#include <wx/wx.h>
// clang-format on

int WXDLLIMPEXP_SDK clMessageBox(const wxString& message,
                                 const wxString& caption = "CodeLite",
                                 long style = wxOK | wxCENTRE,
                                 wxWindow* parent = nullptr,
                                 int x = wxDefaultCoord,
                                 int y = wxDefaultCoord);

#endif // WXCUSTOMCONTROLS_HPP
