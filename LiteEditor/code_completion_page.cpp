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
#include "ColoursAndFontsManager.h"
#include "globals.h"
#include "localworkspace.h"
#include <wx/tokenzr.h>

CodeCompletionPage::CodeCompletionPage(wxWindow* parent, int type)
    : CodeCompletionBasePage(parent)
    , m_type(type)
    , m_ccChanged(false)
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->Apply(m_textCtrlSearchPaths);
    lexer->Apply(m_textCtrlMacros);

    if(m_type == TypeWorkspace) {

        wxArrayString excludePaths, includePaths;
        wxString opts, c_opts;
        wxString macros;

        LocalWorkspace* lw = clCxxWorkspaceST::Get()->GetLocalWorkspace();
        lw->GetParserPaths(includePaths, excludePaths);
        lw->GetParserMacros(macros);

        m_textCtrlSearchPaths->SetValue(wxImplode(includePaths, wxT("\n")));
        m_textCtrlMacros->SetValue(macros);

        if(lw->GetParserFlags() & LocalWorkspace::EnableCpp11) {
            m_choiceStandard->SetStringSelection("C++11");
        } else if(lw->GetParserFlags() & LocalWorkspace::EnableCpp14) {
            m_choiceStandard->SetStringSelection("C++14");
        } else if(lw->GetParserFlags() & LocalWorkspace::EnableCpp17) {
            m_choiceStandard->SetStringSelection("C++17");
        } else if(lw->GetParserFlags() & LocalWorkspace::EnableCpp20) {
            m_choiceStandard->SetStringSelection("C++20");
        }
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

        std::unordered_map<wxString, int> M = { { "C++11", LocalWorkspace::EnableCpp11 },
                                                { "C++14", LocalWorkspace::EnableCpp14 },
                                                { "C++17", LocalWorkspace::EnableCpp17 },
                                                { "C++20", LocalWorkspace::EnableCpp20 } };
        wxString standard = m_choiceStandard->GetStringSelection();
        if(M.count(standard)) {
            flags |= M[standard];
        }

        if(m_checkBoxSWTLW->IsChecked())
            flags |= LocalWorkspace::EnableSWTLW;
        lw->SetParserFlags(flags);
        lw->Flush();
    }
}

void CodeCompletionPage::OnCCContentModified(wxStyledTextEvent& event) { m_ccChanged = true; }
bool CodeCompletionPage::IsCpp11Enabled() const { return m_choiceStandard->GetStringSelection() == "C++11"; }
