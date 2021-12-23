//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : renamefiledlg.cpp
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

#include "renamefiledlg.h"

#include "windowattrmanager.h"

RenameFileDlg::RenameFileDlg(wxWindow* parent, const wxString& replaceWith, std::vector<IncludeStatement>& matches)
    : RenameFileBaseDlg(parent)
{
    m_textCtrlReplaceWith->SetValue(replaceWith);

    for(size_t i = 0; i < matches.size(); i++) {
        wxString displayString;
        IncludeStatement is = matches.at(i);

        displayString << wxString(is.includedFrom.c_str(), wxConvUTF8) << wxT(":") << is.line;
        int idx = m_checkListMatches->Append(displayString);

        // Keep the information about this entry
        m_entries[idx] = is;
        m_checkListMatches->Check(idx);
    }

    if(m_checkListMatches->GetCount()) {
        m_checkListMatches->Select(0);
        DoSelectItem(0);
    }
    SetName("RenameFileDlg");
    WindowAttrManager::Load(this);
}

void RenameFileDlg::OnFileSelected(wxCommandEvent& event) { DoSelectItem(event.GetSelection()); }

RenameFileDlg::~RenameFileDlg() {}

void RenameFileDlg::DoSelectItem(int idx)
{
    std::map<int, IncludeStatement>::iterator iter = m_entries.find(idx);
    if(iter != m_entries.end()) {
        IncludeStatement ise = iter->second;
        wxString line;
        line << ise.line;
        m_staticTextFoundInLine->SetLabel(line);

        m_staticTextIncludedInFile->SetLabel(wxString(ise.includedFrom.c_str(), wxConvUTF8));
        m_staticTextPattern->SetLabel(
            wxString::Format(wxT("#include %s"), wxString(ise.pattern.c_str(), wxConvUTF8).c_str()));
    }
}

std::vector<IncludeStatement> RenameFileDlg::GetMatches() const
{
    std::vector<IncludeStatement> matches;
    for(unsigned int i = 0; i < m_checkListMatches->GetCount(); i++) {
        if(m_checkListMatches->IsChecked(i)) {
            std::map<int, IncludeStatement>::const_iterator iter = m_entries.find((int)i);
            if(iter != m_entries.end()) {
                matches.push_back(iter->second);
            }
        }
    }
    return matches;
}

wxString RenameFileDlg::GetReplaceWith() const { return m_textCtrlReplaceWith->GetValue(); }
