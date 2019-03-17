//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : code_completion_page.cpp
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

#include "code_completion_page.h"
#include "localworkspace.h"
#include <wx/tokenzr.h>
#include "globals.h"

CodeCompletionPage::CodeCompletionPage(wxWindow* parent, int type)
    : CodeCompletionBasePage(parent)
    , m_type(type)
    , m_ccChanged(false)
{
    if(m_type == TypeWorkspace) {

        wxArrayString excludePaths, includePaths;
        wxString opts, c_opts;
        wxString macros;

        LocalWorkspace* lw = clCxxWorkspaceST::Get()->GetLocalWorkspace();
        lw->GetParserPaths(includePaths, excludePaths);
        lw->GetParserMacros(macros);

        m_textCtrlSearchPaths->SetValue(wxImplode(includePaths, wxT("\n")));
        m_textCtrlMacros->SetValue(macros);

        m_checkBoxCpp11->SetValue(lw->GetParserFlags() & LocalWorkspace::EnableCpp11);
        m_checkBoxCpp14->SetValue(lw->GetParserFlags() & LocalWorkspace::EnableCpp14);
        m_checkBoxCpp17->SetValue(lw->GetParserFlags() & LocalWorkspace::EnableCpp17);
        m_checkBoxSWTLW->SetValue(lw->GetParserFlags() & LocalWorkspace::EnableSWTLW);
    }
}

CodeCompletionPage::~CodeCompletionPage() {}

wxArrayString CodeCompletionPage::GetIncludePaths() const
{
    return wxStringTokenize(m_textCtrlSearchPaths->GetValue(), wxT("\n\r"), wxTOKEN_STRTOK);
}

wxString CodeCompletionPage::GetMacros() const { return m_textCtrlMacros->GetValue(); }

wxString CodeCompletionPage::GetIncludePathsAsString() const { return m_textCtrlSearchPaths->GetValue(); }

void CodeCompletionPage::Save()
{
    if(m_type == TypeWorkspace) {
        size_t flags = 0;
        LocalWorkspace* lw = clCxxWorkspaceST::Get()->GetLocalWorkspace();
        lw->SetParserPaths(GetIncludePaths(), wxArrayString());
        lw->SetParserMacros(GetMacros());

        if(m_checkBoxCpp11->IsChecked()) flags |= LocalWorkspace::EnableCpp11;
        if(m_checkBoxCpp14->IsChecked()) flags |= LocalWorkspace::EnableCpp14;
        if(m_checkBoxCpp17->IsChecked()) flags |= LocalWorkspace::EnableCpp17;
        if(m_checkBoxSWTLW->IsChecked()) flags |= LocalWorkspace::EnableSWTLW;
        lw->SetParserFlags(flags);
        lw->Flush();
    }
}

void CodeCompletionPage::OnCCContentModified(wxCommandEvent& event) { m_ccChanged = true; }
