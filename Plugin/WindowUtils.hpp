#pragma once

#include "codelite_exports.h"

class wxWindow;

/// macOS only: give a top level popup window a drop shadow and rounded corners.
/// Call it after the popup is shown (the native window must exist)
WXDLLIMPEXP_SDK void StylePopupWindow(wxWindow* popup, double radius);
