#pragma once

#include <wx/version.h>

//-------------------
// Windows
//-------------------
#define CL_USE_NATIVEBOOK 0
#define MAINBOOK_AUIBOOK 1
#define USE_NATIVETOOLBAR 0

#ifdef USE_POSIX_LAYOUT
#ifndef PLUGINS_DIR
#define PLUGINS_DIR "\\lib\\codelite"
#endif
#ifndef INSTALL_DIR
#define INSTALL_DIR "\\share\\codelite"
#endif
#endif

// General macros
#define CL_USE_NEW_BUILD_TAB 1
#define CL_N0_OF_BOOKMARK_TYPES 5
