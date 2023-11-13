//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : znSettingsDlg.cpp
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

#include "znSettingsDlg.h"

#include "cl_config.h"
#include "event_notifier.h"
#include "windowattrmanager.h"
#include "zn_config_item.h"

wxDEFINE_EVENT(wxEVT_ZN_SETTINGS_UPDATED, wxCommandEvent);

znSettingsDlg::znSettingsDlg(wxWindow* parent)
    : znSettingsDlgBase(parent)
{
    znConfigItem data;
    clConfig conf("zoom-navigator.conf");
    if(conf.ReadItem(&data)) {
        m_checkBoxEnableZN->SetValue(data.IsEnabled());
        m_colourPickerHighlightColour->SetColour(wxColour(data.GetHighlightColour()));
        m_checkBoxUseVScrollbar->SetValue(data.IsUseScrollbar());
    }

    m_spinCtrlZoomFactor->SetValue(data.GetZoomFactor());
    GetSizer()->Fit(this);
    CentreOnParent();
}

znSettingsDlg::~znSettingsDlg() {}

void znSettingsDlg::OnOK(wxCommandEvent& event)
{
    znConfigItem data;
    clConfig conf("zoom-navigator.conf");
    data.SetEnabled(m_checkBoxEnableZN->IsChecked());
    data.SetHighlightColour(m_colourPickerHighlightColour->GetColour().GetAsString(wxC2S_HTML_SYNTAX));
    data.SetUseScrollbar(m_checkBoxUseVScrollbar->IsChecked());
    data.SetZoomFactor(m_spinCtrlZoomFactor->GetValue());
    conf.WriteItem(&data);

    // notify thet the settings were changed
    wxCommandEvent evt(wxEVT_ZN_SETTINGS_UPDATED);
    EventNotifier::Get()->AddPendingEvent(evt);

    EndModal(wxID_OK);
}
