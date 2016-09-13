//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2016 The CodeLite Team
// File name            : AutoSaveDlg.h
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

#ifndef AUTOSAVEDLG_H
#define AUTOSAVEDLG_H
#include "AutoSaveUI.h"

class AutoSaveDlg : public AutoSaveDlgBase
{
public:
    AutoSaveDlg(wxWindow* parent);
    virtual ~AutoSaveDlg();
protected:
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnEnabledUI(wxUpdateUIEvent& event);
};
#endif // AUTOSAVEDLG_H
