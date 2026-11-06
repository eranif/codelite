#pragma once

#include "codelite_exports.h"

class wxWindow;

/// macOS and Windows: give a top level popup window a drop shadow and rounded corners. Does nothing on other platforms.
/// Call it after the popup is shown (the native window must exist).
/// On Windows 11 the system draws the corners and supports two sizes only (about 8 and 4 pixels at 100% scaling), so
/// `radius` selects the closest one. On older versions of Windows the window is clipped to a rounded rectangle and it
/// has no shadow.
WXDLLIMPEXP_SDK void StylePopupWindow(wxWindow* popup, double radius);
