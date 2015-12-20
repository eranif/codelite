//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : quick_outline_dlg.h
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

#ifndef __quick_outline_dlg__
#define __quick_outline_dlg__

#include "php_ui.h"
class IEditor;
class IManager;
class PHPEntry;

class PHPQuickOutlineDlg : public QuickOutlineDlgBase
{
protected:
    IEditor* m_editor;
    IManager* m_mgr;

protected:
    // Handlers for QuickOutlineDlgBase events.
    void OnKeyDown(wxKeyEvent& event);
    void OnItemActivated(wxTreeEvent& event);

protected:
    void DoItemSelected(const wxTreeItemId& item);
    void DoSelectMatch(const wxString& filename, int line, const wxString& what);

public:
    PHPQuickOutlineDlg(wxWindow* parent, IEditor* editor, IManager* manager);
    virtual ~PHPQuickOutlineDlg();
};

#endif // __quick_outline_dlg__
