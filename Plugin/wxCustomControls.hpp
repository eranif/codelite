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
#endif // WXCUSTOMCONTROLS_HPP
