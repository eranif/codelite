//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : FileMappingDlg.h
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

#ifndef FILEMAPPINGDLG_H
#define FILEMAPPINGDLG_H
#include "php_ui.h"

class FileMappingDlg : public FileMappingDlgBase
{
public:
    FileMappingDlg(wxWindow* parent);
    virtual ~FileMappingDlg() = default;

    wxString GetSourceFolder() const {
        return m_dirPickerSource->GetPath();
    }
    
    wxString GetTargetFolder() const {
        return m_textCtrlRemote->GetValue();
    }
    
    void SetSourceFolder( const wxString &path) {
        m_dirPickerSource->SetPath( path );
    }
    
    void SetTargetFolder( const wxString &path) {
        m_textCtrlRemote->ChangeValue( path );
    }
    
protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
};
#endif // FILEMAPPINGDLG_H
