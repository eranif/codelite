//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : uisettingsdlg.cpp
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

#include "imanager.h"
#include "callgraph.h"
#include "uisettingsdlg.h"
#include <wx/filedlg.h>
#include "windowattrmanager.h"

uisettingsdlg::uisettingsdlg(wxWindow* parent, IManager* mgr) //, CallGraph *plugin )
    : uisettings(parent),
      m_mgr(mgr)
{
    // ConfCallGraph confData;
    // read last stored value for paths gprof and dot
    m_mgr->GetConfigTool()->ReadObject(wxT("CallGraph"), &confData);
    m_textCtrl_path_gprof->SetValue(confData.GetGprofPath());
    m_textCtrl_path_dot->SetValue(confData.GetDotPath());
    m_spinCtrl_treshold_edge->SetValue(confData.GetTresholdEdge());
    m_spinCtrl_treshold_node->SetValue(confData.GetTresholdNode());
    m_spinCtrl_colors_node->SetValue(confData.GetColorsNode());
    m_spinCtrl_colors_edge->SetValue(confData.GetColorsEdge());
    m_checkBox_Names->SetValue(confData.GetHideParams());
    m_checkBox_Parameters->SetValue(confData.GetStripParams());
    m_checkBox_Namespaces->SetValue(confData.GetHideNamespaces());
    //
    if(m_checkBox_Names->IsChecked()) m_checkBox_Parameters->Disable();
    if(m_checkBox_Parameters->IsChecked()) m_checkBox_Names->Disable();
    //

    GetSizer()->Fit(this);
    SetName("uisettingsdlg");
    WindowAttrManager::Load(this);
}

void uisettingsdlg::OnButton_click_select_gprof(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString new_gprof_path = wxFileSelector(_("Select gprof..."),
                                             m_textCtrl_path_gprof->GetValue().c_str(),
                                             wxT(""),
                                             wxT(""),
                                             wxFileSelectorDefaultWildcardStr,
                                             0,
                                             this);
    if(!new_gprof_path.IsEmpty()) {
        m_textCtrl_path_gprof->SetValue(new_gprof_path);
    }
}

void uisettingsdlg::OnButton_click_select_dot(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString new_dot_path = wxFileSelector(_("Select dot..."),
                                           m_textCtrl_path_dot->GetValue().c_str(),
                                           wxT(""),
                                           wxT(""),
                                           wxFileSelectorDefaultWildcardStr,
                                           0,
                                           this);
    if(!new_dot_path.IsEmpty()) {
        m_textCtrl_path_dot->SetValue(new_dot_path);
    }
}

void uisettingsdlg::OnButton_click_ok(wxCommandEvent& event)
{
    wxUnusedVar(event);
    // store values
    // ConfCallGraph confData;
    confData.SetGprofPath(m_textCtrl_path_gprof->GetValue());
    confData.SetDotPath(m_textCtrl_path_dot->GetValue());
    confData.SetTresholdNode(m_spinCtrl_treshold_node->GetValue());
    confData.SetTresholdEdge(m_spinCtrl_treshold_edge->GetValue());
    confData.SetColorsEdge(m_spinCtrl_colors_edge->GetValue());
    confData.SetColorsNode(m_spinCtrl_colors_node->GetValue());
    confData.SetHideParams(m_checkBox_Names->GetValue());
    confData.SetHideNamespaces(m_checkBox_Namespaces->GetValue());
    confData.SetStripParams(m_checkBox_Parameters->GetValue());

    m_mgr->GetConfigTool()->WriteObject(wxT("CallGraph"), &confData);

    if((wxFileExists(m_textCtrl_path_gprof->GetValue())) && (wxFileExists(m_textCtrl_path_dot->GetValue()))) {
        EndModal(wxID_OK);
    } else {
        wxMessageBox(_("Please check the external tools' paths settings."),
                     wxT("CallGraph"),
                     wxOK | wxICON_ERROR,
                     m_mgr->GetTheApp()->GetTopWindow());
    }

    // event.Skip();
}

void uisettingsdlg::OnButton_click_cancel(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_CANCEL);
}

void uisettingsdlg::OnCheckName(wxCommandEvent& event)
{
    if(m_checkBox_Names->IsChecked())
        m_checkBox_Parameters->Disable();
    else
        m_checkBox_Parameters->Enable();
}

void uisettingsdlg::OnCheckParam(wxCommandEvent& event)
{
    if(m_checkBox_Parameters->IsChecked())
        m_checkBox_Names->Disable();
    else
        m_checkBox_Names->Enable();
}
