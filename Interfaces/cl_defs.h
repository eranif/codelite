#ifndef CL_DEFS_H
#define CL_DEFS_H

//-----------------------------------
// Use new toolbars where possible
//-----------------------------------
#include <wx/version.h>

#if defined(__WXMSW__)||defined(__WXMAC__)||defined(__WXGTK__)
#    if wxCHECK_VERSION(2, 8, 9)
#        define clToolBar          wxAuiToolBar
#        define clTB_DEFAULT_STYLE wxAUI_TB_DEFAULT_STYLE
#        define USE_AUI_TOOLBAR    1
#    else
#        define clToolBar          wxToolBar
#        define clTB_DEFAULT_STYLE wxTB_FLAT | wxTB_NODIVIDER
#        define USE_AUI_TOOLBAR    0
#    endif
#else // !Mac !Win !Linux
#    define clToolBar          wxToolBar
#    define clTB_DEFAULT_STYLE wxTB_FLAT | wxTB_NODIVIDER
#    define USE_AUI_TOOLBAR    0
#endif

#ifdef __WXGTK__
#    ifndef PLUGINS_DIR
#        define PLUGINS_DIR "/usr/lib/codelite"
#    endif
#    ifndef INSTALL_DIR
#        define INSTALL_DIR "/usr/share/codelite"
#    endif
#    define CL_USE_NATIVEBOOK 1
#else
#    define CL_USE_NATIVEBOOK 0
#endif

#if wxVERSION_NUMBER < 2904
#    define CL_USE_NEW_BUILD_TAB 0
#else
#    define CL_USE_NEW_BUILD_TAB 1
#endif

#endif // CL_DEFS_H
