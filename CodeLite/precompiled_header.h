//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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

#ifndef PRECOMPILED_HEADER_H
#define PRECOMPILED_HEADER_H

// A proxy to the right PCH file
// we need to so we will get a different flags compiled per file
#if !defined(_WIN64)
#   include "../PCH/precompiled_header_release_32.h"
#else
#ifdef NDEBUG
#    include "../PCH/precompiled_header_release.h"
#else
#    include "../PCH/precompiled_header_dbg.h"
#endif
#endif
#endif

