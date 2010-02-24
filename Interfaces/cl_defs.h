#ifndef CL_DEFS_H
#define CL_DEFS_H

//-----------------------------------
// Use new toolbars where possible
//-----------------------------------

#if defined(__WXMSW__)||defined(__WXMAC__)||defined(__WXGTK__)
#    if wxABI_VERSION >= 20809
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


#endif // CL_DEFS_H
