//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : SFTPSettingsDialog.h
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

#ifndef SFTPSETTINGSDIALOG_H
#define SFTPSETTINGSDIALOG_H
#include "UI.h"

class SFTPSettingsDialog : public SFTPSettingsDialogBase
{
public:
    SFTPSettingsDialog(wxWindow* parent);
    virtual ~SFTPSettingsDialog() = default;

protected:
    virtual void OnOK(wxCommandEvent& event);
};
#endif // SFTPSETTINGSDIALOG_H
