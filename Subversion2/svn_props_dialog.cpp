//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svn_props_dialog.cpp
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

#include "svn_props_dialog.h"
#include "svn_local_properties.h"
#include "windowattrmanager.h"
#include "subversion2.h"

SvnPropsDlg::SvnPropsDlg(wxWindow* parent, const wxString& url, Subversion2* plugin)
    : SvnPropsBaseDlg(parent)
    , m_plugin(plugin)
    , m_url(url)
{
    m_staticTextURL->SetLabel(m_url);

    SubversionLocalProperties props(m_url);
    m_textCtrlBugURL->SetValue(props.ReadProperty(SubversionLocalProperties::BUG_TRACKER_URL));
    m_textCtrlFrURL->SetValue(props.ReadProperty(SubversionLocalProperties::FR_TRACKER_URL));
    m_textCtrlBugMsg->SetValue(props.ReadProperty(SubversionLocalProperties::BUG_TRACKER_MESSAGE));
    m_textCtrlFrMsg->SetValue(props.ReadProperty(SubversionLocalProperties::FR_TRACKER_MESSAGE));
    SetName("SvnPropsDlg");
    CentreOnParent();
    GetSizer()->Fit(this);
}

SvnPropsDlg::~SvnPropsDlg() {}

wxString SvnPropsDlg::GetBugTrackerURL() const { return m_textCtrlBugURL->GetValue(); }

wxString SvnPropsDlg::GetFRTrackerURL() const { return m_textCtrlFrURL->GetValue(); }

wxString SvnPropsDlg::GetBugMsg() const { return m_textCtrlBugMsg->GetValue(); }

wxString SvnPropsDlg::GetFRMsg() const { return m_textCtrlFrMsg->GetValue(); }
