//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : ImportFilesDialogNew.h
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

#ifndef IMPORTFILESDIALOGNEW_H
#define IMPORTFILESDIALOGNEW_H

#include "importfilesdialog_new.h"
#include <wx/dir.h>
#include <map>
#include "macros.h"

class ImportFilesDialogNew : public ImportFilesDialogNewBase
{
    wxDataViewItem m_root;
    wxVariant MakeIconText(const wxString &text, const wxBitmap& bmp) const;
    
protected:
    virtual void OnBrowse(wxCommandEvent& event);
    virtual void OnItemExpanding(wxDataViewEvent& event);
    void DoCheckChildren(const wxDataViewItem& parent, bool check);
    void DoGetCheckedDirs(const wxDataViewItem& parent, wxStringBoolMap_t &dirs);

protected:
    virtual void OnValueChanged(wxDataViewEvent& event);
    virtual void OnDirChanged(wxCommandEvent& event);
    void DoBuildTree(const wxDataViewItem& parent, const wxDir& dir, bool initialState);
    void DoBuildTree();

public:
    ImportFilesDialogNew(wxWindow* parent);
    virtual ~ImportFilesDialogNew();
    void GetDirectories(wxStringBoolMap_t &dirs);
    bool ExtlessFiles();
    wxString GetFileMask();
    wxString GetBaseDir();

};
#endif // IMPORTFILESDIALOGNEW_H
