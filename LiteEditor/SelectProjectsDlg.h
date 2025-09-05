//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SelectProjectsDlg.h
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

#ifndef SELECTPROJECTSDLG_H
#define SELECTPROJECTSDLG_H
#include "rename_symbool_dlg.h"

class SelectProjectsDlg : public SelectProjectsDlgBase
{
public:
    SelectProjectsDlg(wxWindow* parent);
    virtual ~SelectProjectsDlg() = default;
    wxArrayString GetProjects() const;
    
protected:
    virtual void OnSelectAll(wxCommandEvent& event);
    virtual void OnUnSelectAll(wxCommandEvent& event);
};
#endif // SELECTPROJECTSDLG_H
