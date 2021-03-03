//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : new_item_dlg.h
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

#ifndef __new_item_dlg__
#define __new_item_dlg__

#include "new_item_base_dlg.h"
#include <wx/filename.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class NewItemDlg
///////////////////////////////////////////////////////////////////////////////
class NewItemDlg : public NewItemBaseDlg
{
public:
    NewItemDlg(wxWindow* parent, const wxString& cwd);
    virtual ~NewItemDlg();
    wxFileName GetFileName() const;

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnBrowseButton(wxCommandEvent& event);
};

#endif //__new_item_dlg__
