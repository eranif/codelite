//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : HelpPluginSettingsDlg.h
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

#ifndef HELPPLUGINSETTINGSDLG_H
#define HELPPLUGINSETTINGSDLG_H
#include "HelpPluginUI.h"

class HelpPluginSettingsDlg : public HelpPluginSettingsDlgBase
{
    bool m_modified;
public:
    HelpPluginSettingsDlg(wxWindow* parent);
    virtual ~HelpPluginSettingsDlg();

protected:
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnDocsetChanged(wxPropertyGridEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // HELPPLUGINSETTINGSDLG_H
