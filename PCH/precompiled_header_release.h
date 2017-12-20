//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : precompiled_header.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef CODELITE_PRECOMPILED_HEADER_RELEASE_H
#define CODELITE_PRECOMPILED_HEADER_RELEASE_H

#ifdef __cplusplus

// Only include the headers we need here
#include "../CodeLite/MSWPrivate.h"
#include "../CodeLite/cl_defs.h"
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/string.h>
#include <wx/process.h>
#include <wx/filename.h>
#include <wx/treectrl.h>
#include <wx/string.h>
#include <wx/log.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <set>
#include <list>
#include <wx/fontmap.h>
#include <wx/clipbrd.h>
#include <wx/numdlg.h>
#include <wx/stdpaths.h>
#include <wx/xrc/xmlres.h>
#include <wx/splitter.h>
#include <wx/aboutdlg.h>
#include <wx/aui/framemanager.h>
#include <wx/ffile.h>
#include <wx/dir.h>
#include <wx/socket.h>
#include <wx/splash.h>
#include <wx/fileconf.h>
#include <wx/xrc/xmlres.h>
#include <wx/sysopt.h>
#include "wx/txtstrm.h"
#include "wx/wfstream.h"
#include <wx/cmdline.h>
#include "wx/tokenzr.h"
#include "wx/dir.h"
#include <wx/stdpaths.h>
#include <wx/snglinst.h>
#include <wx/image.h>
#include <wx/filefn.h>
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <wx/fontmap.h>
#include <wx/xrc/xmlres.h>
#include <wx/ffile.h>
#include <wx/tooltip.h>
#include <wx/settings.h>
#include <wx/fdrepdlg.h>
#include "wx/tokenzr.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <wx/wupdlock.h>

/////////////////////////////////////////////////////////////////////////////
// Unicode:
// Use the following macros, in the following cases:
//
// 1. If your API requires const char*, wrap wxString with _C() macro
// 2. To convert char* ( not litteral ) use _U()
// 3. To convert literal strings, use _T()
/////////////////////////////////////////////////////////////////////////////

#ifdef __APPLE__
    #include <ctype.h>
    #undef _C /* _CTYPE_C */
    #undef _U /* _CTYPE_U */
#endif

#ifdef wxUSE_UNICODE
    #ifndef _U
        #define _U(x) wxString((x),wxConvUTF8)
    #endif
    #ifndef _UU
        #define _UU(x,y) wxString((x),y)
    #endif
    #ifndef _CC
        #define _CC(x,y) (x).mb_str((y))
    #endif
#else
    #ifndef _U
        #define _U(x) (x)
    #endif
    #ifndef _UU
        #define _UU(x,y) (x)
    #endif
    #ifndef _CC
        #define _CC(x,y) (x)
    #endif
#endif

#ifndef _C
    #define _C(x) _CC((x),wxConvUTF8)
#endif

#endif // __cplusplus
#endif // CODELITE_PRECOMPILED_HEADER_RELEASE_H
