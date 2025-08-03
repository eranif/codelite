//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php_settings_dlg.h
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

#ifndef __php_settings_dlg__
#define __php_settings_dlg__

#include "php_ui.h"

class PHPSettingsDlg : public PHPSettingsBaseDlg
{
protected:
    // Handlers for PHPSettingsBaseDlg events.
    void OnBrowseForIncludePath(wxCommandEvent& event);
    void OnAddCCPath(wxCommandEvent& event);
    void OnUpdateApplyUI(wxCommandEvent& event);

public:
    PHPSettingsDlg(wxWindow* parent);
    virtual ~PHPSettingsDlg() = default;
    void OnOK(wxCommandEvent& event);
};

#endif // __php_settings_dlg__
