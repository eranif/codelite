//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : pluginmgrdlg.h
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
#ifndef __pluginmgrdlg__
#define __pluginmgrdlg__

#include "plugindlgbase.h"

class PluginMgrDlg : public PluginMgrDlgBase
{
    wxArrayString m_initialEnabledPlugins;

protected:
    virtual void OnCheckAll(wxCommandEvent& event);
    virtual void OnCheckAllUI(wxUpdateUIEvent& event);
    virtual void OnUncheckAll(wxCommandEvent& event);
    virtual void OnUncheckAllUI(wxUpdateUIEvent& event);
    void WritePropertyLine(const wxString& label, const wxString& text);
    void Initialize();
    void OnItemSelected(wxDataViewEvent& event);
    void OnButtonOK(wxCommandEvent& event);
    void CreateInfoPage(unsigned int index);

public:
    /** Constructor */
    PluginMgrDlg(wxWindow* parent);
    virtual ~PluginMgrDlg();
};

#endif // __pluginmgrdlg__
