//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : continuousbuildpane.cpp
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

#include "continuousbuildpane.h"

#include "continuousbuild.h"
#include "continuousbuildconf.h"
#include "drawingutils.h"
#include "imanager.h"

ContinuousBuildPane::ContinuousBuildPane(wxWindow* parent, IManager* manager, ContinuousBuild* plugin)
    : ContinuousBuildBasePane(parent)
    , m_mgr(manager)
    , m_plugin(plugin)
{
    ContinuousBuildConf conf;
    m_mgr->GetConfigTool()->ReadObject(wxT("ContinousBuildConf"), &conf);
    m_checkBox1->SetValue(conf.GetEnabled());

    m_listBoxQueue->SetForegroundColour(DrawingUtils::GetOutputPaneFgColour());
    m_listBoxQueue->SetBackgroundColour(DrawingUtils::GetOutputPaneBgColour());
}

void ContinuousBuildPane::OnStopAll(wxCommandEvent& event)
{
    m_listBoxQueue->Clear();
    m_listBoxFailedFiles->Clear();
    m_plugin->StopAll();
}

void ContinuousBuildPane::OnStopUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_listBoxQueue->IsEmpty() /* || !m_listBoxFailedFiles->IsEmpty()*/);
}

void ContinuousBuildPane::RemoveFile(const wxString& file)
{
    int where = m_listBoxQueue->FindString(file);
    if(where != wxNOT_FOUND) {
        m_listBoxQueue->Delete((unsigned int)where);
    }
}

void ContinuousBuildPane::AddFile(const wxString& file)
{
    if(m_listBoxQueue->FindString(file) == wxNOT_FOUND) {
        m_listBoxQueue->Append(file);
    }
}

void ContinuousBuildPane::ClearAll()
{
    m_listBoxQueue->Clear();
    m_listBoxFailedFiles->Clear();
}

void ContinuousBuildPane::AddFailedFile(const wxString& file)
{
    if(m_listBoxFailedFiles->FindString(file) == wxNOT_FOUND) {
        m_listBoxFailedFiles->Append(file);
    }
}

void ContinuousBuildPane::OnEnableContBuildUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBox1->IsChecked());
}

void ContinuousBuildPane::OnEnableCB(wxCommandEvent& event)
{
    ContinuousBuildConf conf;
    conf.SetEnabled(event.IsChecked());
    m_mgr->GetConfigTool()->WriteObject(wxT("ContinousBuildConf"), &conf);
}
