//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_aui_tool_stickness.cpp
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

#include "cl_aui_tool_stickness.h"

clAuiToolStickness::clAuiToolStickness(wxAuiToolBar* tb, int toolId)
    : m_tb(tb)
    , m_item(nullptr)
{
    if(m_tb) {
        m_item = m_tb->FindTool(toolId);
    }

    if(m_item) {
        m_item->SetSticky(true);
    }
}

clAuiToolStickness::~clAuiToolStickness()
{
    if(m_item) {
        m_item->SetSticky(false);
    }

    if(m_tb) {
        m_tb->Refresh();
    }
}
