//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : cl_aui_tool_stickness.h
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

#ifndef CLAUITOOLSTICKNESS_H
#define CLAUITOOLSTICKNESS_H

#include "codelite_exports.h"

#include <wx/aui/auibar.h>
#include <wx/bitmap.h>

class WXDLLIMPEXP_SDK clAuiToolStickness
{
    wxAuiToolBar* m_tb;
    wxAuiToolBarItem* m_item;

public:
    clAuiToolStickness(wxAuiToolBar* tb, int toolId);
    virtual ~clAuiToolStickness();
};

#endif // CLAUITOOLSTICKNESS_H
