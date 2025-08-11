//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : FilesModifiedDlg.h
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

#ifndef FILESMODIFIEDDLG_H
#define FILESMODIFIEDDLG_H
#include "filechecklistbase.h"

class FilesModifiedDlg : public FilesModifiedDlgBase
{
public:
    enum {
        kID_BUTTON_CHOOSE = ID_BUTTON_CHOOSE,
        kID_BUTTON_LOAD   = ID_BUTTON_LOAD,
        kID_BUTTON_IGNORE = ID_BUTTON_IGNORE,
    };
    
public:
    FilesModifiedDlg(wxWindow* parent);
    virtual ~FilesModifiedDlg() = default;

    bool GetRememberMyAnswer() const {
        return m_checkBoxRemember->IsChecked();
    }
protected:
    virtual void OnIgnore(wxCommandEvent& event);
    virtual void OnChoose(wxCommandEvent& event);
    virtual void OnLoad(wxCommandEvent& event);
};
#endif // FILESMODIFIEDDLG_H
