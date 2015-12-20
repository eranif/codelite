//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPDebugStartDlg.h
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

#ifndef PHPDEBUGSTARTDLG_H
#define PHPDEBUGSTARTDLG_H
#include "php_ui.h"
#include "php_project.h"

class PHPDebugStartDlg : public PHPDebugStartDlgBase
{
    PHPProject::Ptr_t m_project;
    IManager* m_manager;
protected:
    void SetBookSelection(int sel) { m_simpleBook->SetSelection(sel); }
    
public:
    PHPDebugStartDlg(wxWindow* parent, PHPProject::Ptr_t pProject, IManager* manager);
    virtual ~PHPDebugStartDlg();
    wxString GetPath() const;
    
protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
    virtual void OnUseActiveEditor(wxCommandEvent& event);
    virtual void OnDebugMethodChanged(wxCommandEvent& event);
    virtual void OnScriptToDebugUI(wxUpdateUIEvent& event);
};
#endif // PHPDEBUGSTARTDLG_H
