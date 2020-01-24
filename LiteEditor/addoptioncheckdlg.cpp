//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : addoptioncheckdlg.cpp
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

#include "addoptioncheckdlg.h"
#include "windowattrmanager.h"
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/sstream.h>
#include <list>
#include <algorithm> // std::find
#include <globals.h>

AddOptionCheckDlg::AddOptionCheckDlg(wxWindow* parent,
                                     const wxString& title,
                                     const Compiler::CmpCmdLineOptions& cmpOptions,
                                     const wxString& value)
    : AddOptionCheckDialogBase(parent, wxID_ANY, title)
    , m_cmpOptions(cmpOptions)
{
    SetName("AddOptionCheckDlg");
    WindowAttrManager::Load(this);

    // Fill the list of available options
    Compiler::CmpCmdLineOptions::const_iterator itOption = m_cmpOptions.begin();
    for(; itOption != m_cmpOptions.end(); ++itOption) {
        const Compiler::CmpCmdLineOption& cmpOption = itOption->second;
        m_checkListOptions->Append(cmpOption.help + wxT(" [") + cmpOption.name + wxT("]"));
    }

    // Update controls
    SetValue(value);

    clSetSmallDialogBestSizeAndPosition(this);
}

AddOptionCheckDlg::~AddOptionCheckDlg() {}

void AddOptionCheckDlg::SetValue(const wxString& value)
{
    m_textCmdLn->SetValue(value);

    UpdateOptions();
    UpdateCmdLine();
}

wxString AddOptionCheckDlg::GetValue() const { return m_textCmdLn->GetValue(); }

void AddOptionCheckDlg::OnOptionToggled(wxCommandEvent& event)
{
    UpdateCmdLine();
    UpdateOptions();
}

void AddOptionCheckDlg::OnOptionsText(wxCommandEvent& event) { UpdateCmdLine(); }

void AddOptionCheckDlg::UpdateOptions()
{
    // Remove all check boxes
    m_checkListOptions->Freeze();

    for(unsigned int idx = 0; idx < m_checkListOptions->GetCount(); ++idx) {
        m_checkListOptions->Check(idx, false);
    }

    // Check all options entered
    wxString customOptions;
    wxStringTokenizer tkz(m_textCmdLn->GetValue(), wxT(";"));
    while(tkz.HasMoreTokens()) {
        wxString token = tkz.GetNextToken();
        token = token.Trim().Trim(false);
        if(!token.empty()) {
            Compiler::CmpCmdLineOptions::const_iterator itOption = m_cmpOptions.find(token);
            if(itOption != m_cmpOptions.end()) {
                const Compiler::CmpCmdLineOption& cmpOption = itOption->second;
                m_checkListOptions->Check(
                    m_checkListOptions->FindString(cmpOption.help + wxT(" [") + cmpOption.name + wxT("]")));
            } else {
                if(!customOptions.empty()) customOptions << wxT(";");
                customOptions << token;
            }
        }
    }
    m_textOptions->ChangeValue(customOptions);

    m_checkListOptions->Thaw();
}

void AddOptionCheckDlg::UpdateCmdLine()
{
    // Store all actual options
    std::list<wxString> options;
    wxStringInputStream input(m_textOptions->GetValue());
    wxTextInputStream text(input);
    while(!input.Eof()) {
        wxString option = text.ReadLine().Trim().Trim(false);
        if(!option.empty()) {
            options.push_back(option);
        }
    }

    // Read check box options
    for(unsigned int idx = 0; idx < m_checkListOptions->GetCount(); ++idx) {
        wxString value = m_checkListOptions->GetString(idx).AfterLast(wxT('[')).BeforeLast(wxT(']'));
        if(m_checkListOptions->IsChecked(idx)) {
            // If the option doesn't exist actually, add it
            if(std::find(options.begin(), options.end(), value) == options.end()) {
                // options.push_back(value.AfterLast(wxT('[')).BeforeLast(wxT(']')));
                options.insert(options.begin(), value);
            } else
                // uncheck the option if already defined manualy
                m_checkListOptions->Check(idx, false);
        }
    }

    // Update the options textctrl
    wxString value;
    std::list<wxString>::const_iterator itOption = options.begin();
    for(; itOption != options.end(); ++itOption) {
        if(!value.Contains(*itOption + wxT(";"))) {
            if(!value.empty()) value << wxT(";");
            value << *itOption;
        }
    }
    m_textCmdLn->SetValue(value);
}
