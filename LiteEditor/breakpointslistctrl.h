//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : breakpointdlg.h
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
#ifndef __breakpointslistctrl__
#define __breakpointslistctrl__

#include <vector>
#include "debugger.h"

class BreakpointsListctrl : public wxListCtrl
{
    enum column_ids {
        col_id,
        col_type,
        col_enabled,
        col_file,
        col_lineno,
        col_functionname,
        col_at,
        col_memory,
        col_what,
        col_ignorecount,
        col_extras
    };

public:
    BreakpointsListctrl(wxWindow* parent,
                        wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxLC_REPORT | wxLC_SINGLE_SEL)
        : wxListCtrl(parent, id, pos, size, style)
    {
        const wxString column_headers[] = { _("ID"), _("Type"),   _("Enabled"), _("File"),    _("Line"),  _("Function"),
                                            _("At"), _("Memory"), _("What"),    _("Ignored"), _("Extras") };

        for(int n = col_id; n <= col_extras; ++n) {
            InsertColumn(n, column_headers[n]);
        }

        SetColumnWidth(col_id, 50);
        SetColumnWidth(col_type, 100);
        SetColumnWidth(col_enabled, 100);
        SetColumnWidth(col_file, 300);
        SetColumnWidth(col_lineno, 100);
        SetColumnWidth(col_functionname, 100);
        SetColumnWidth(col_at, 100);
        SetColumnWidth(col_memory, 100);
        SetColumnWidth(col_what, 100);
        SetColumnWidth(col_ignorecount, 50);
        SetColumnWidth(col_extras, 50);
    }

    void Initialise(std::vector<BreakpointInfo>& bps);
    int GetSelection();
    int GetLinenoColumn() { return col_lineno; }
    int GetFileColumn() { return col_file; }
};

#endif //  __breakpointslistctrl__
