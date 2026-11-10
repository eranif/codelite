#pragma once

#include "codelite_exports.h"

class wxWindow;

#ifdef __WXMAC__
constexpr double kDefaultRadius = 5.0;
#else
constexpr double kDefaultRadius = 10.0;
#endif

/// macOS and Windows: give a top level popup window a drop shadow and rounded corners.
/// Call it after the popup is shown (the native window must exist).
/// GTK (3 or newer): rounded corners only (no shadow), and only when a compositor is running. The window must be made
/// transparent, which GTK allows only before the popup is shown: call it before Show() / Popup(). A call on a popup that
/// is already shown does nothing.
/// Other platforms: does nothing.
/// On Windows 11 the system draws the corners and supports two sizes only (about 8 and 4 pixels at 100% scaling), so
/// `radius` selects the closest one. On older versions of Windows the window is clipped to a rounded rectangle and it
/// has no shadow.
WXDLLIMPEXP_SDK void StylePopupWindow(wxWindow* popup, double radius = kDefaultRadius);
