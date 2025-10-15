//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : codelite_exports.h
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

#ifndef CODELITE_EXPORTS
#define CODELITE_EXPORTS

#ifdef __WXMSW__

#include <wx/string.h>
#ifdef WXDLLIMPEXP_CL
#undef WXDLLIMPEXP_CL
#endif

//////////////////////////////////////
// CodeLite
//////////////////////////////////////

#ifdef WXMAKINGDLL_CL
#    define WXDLLIMPEXP_CL __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#    define WXDLLIMPEXP_CL __declspec(dllimport)
#else // not making nor using DLL
#    define WXDLLIMPEXP_CL
#endif

//////////////////////////////////////
// SDK
//////////////////////////////////////
#ifdef WXDLLIMPEXP_SDK
#undef WXDLLIMPEXP_SDK
#endif

#ifdef WXMAKINGDLL_SDK
#    define WXDLLIMPEXP_SDK __declspec(dllexport)
#elif defined(WXUSINGDLL_SDK)
#    define WXDLLIMPEXP_SDK __declspec(dllimport)
#else // not making nor using DLL
#    define WXDLLIMPEXP_SDK
#endif

#else // !MSW
#    define WXDLLIMPEXP_CL
#    define WXDLLIMPEXP_SDK
#endif

#endif // CODELITE_EXPORTS


