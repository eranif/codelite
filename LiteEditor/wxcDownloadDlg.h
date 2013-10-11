//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : wxcDownloadDlg.h
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

#ifndef WXCDOWNLOADDLG_H
#define WXCDOWNLOADDLG_H
#include "wxcrafter.h"

class wxcDownloadDlg : public wxcDownloadDlgBaseClass
{
public:
    wxcDownloadDlg(wxWindow* parent);
    virtual ~wxcDownloadDlg();
protected:
    virtual void OnDownloadWxCrafterPlugin(wxCommandEvent& event);
    virtual void OnIgnoreTheError(wxCommandEvent& event);
};
#endif // WXCDOWNLOADDLG_H
