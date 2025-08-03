//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : ProjectCustomBuildTragetDlg.h
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

#ifndef PROJECTCUSTOMBUILDTRAGETDLG_H
#define PROJECTCUSTOMBUILDTRAGETDLG_H
#include "project_settings_base_dlg.h"

class ProjectCustomBuildTragetDlg : public ProjectCustomBuildTragetDlgBase
{
public:
    static const wxString CUSTOM_TARGET_BUILD               ;
    static const wxString CUSTOM_TARGET_CLEAN               ;
    static const wxString CUSTOM_TARGET_REBUILD             ;
    static const wxString CUSTOM_TARGET_COMPILE_SINGLE_FILE ;
    static const wxString CUSTOM_TARGET_PREPROCESS_FILE     ;

public:
    ProjectCustomBuildTragetDlg(wxWindow* parent, const wxString &targetName, const wxString& targetCommand);
    virtual ~ProjectCustomBuildTragetDlg() = default;

    wxString GetTargetName() const {
        return m_textCtrlTargetName->GetValue();
    }

    wxString GetTargetCommand() const {
        return m_textCtrlCommand->GetValue();
    }
    
    static bool IsPredefinedTarget(const wxString& name);
    
protected:
    virtual void OnEditTargetNameUI(wxUpdateUIEvent& event);
};
#endif // PROJECTCUSTOMBUILDTRAGETDLG_H
