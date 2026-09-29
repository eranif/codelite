//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : NewVirtualFolderDlg.h
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

#ifndef NEWVIRTUALFOLDERDLG_H
#define NEWVIRTUALFOLDERDLG_H

#include "new_virtual_folder.hpp"

class NewVirtualFolderDlg : public NewVirtualFolderDlgBase
{
    wxString m_basePath;

protected:
    void DoUpdatePath();

public:
    NewVirtualFolderDlg(wxWindow* parent, const wxString& currentVD);
    ~NewVirtualFolderDlg() override;
    wxString GetFolderName() const { return m_textCtrlName->GetValue(); }
    bool GetCreateOnDisk() const { return m_checkBoxCreateOnDisk->IsChecked(); }
    wxString GetDiskPath() const { return m_textCtrlPath->GetValue(); }

protected:
    void OnCreateOnFolderChecked(wxCommandEvent& event) override;
    void OnOkUI(wxUpdateUIEvent& event) override;
    void OnCreateOnDiskUI(wxUpdateUIEvent& event) override;
    void OnNameUpdated(wxCommandEvent& event) override;
};
#endif // NEWVIRTUALFOLDERDLG_H
