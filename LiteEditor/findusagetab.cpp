//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : findusagetab.cpp
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

#include "findusagetab.h"
#include "findresultstab.h"
#include <wx/xrc/xmlres.h>
#include <wx/ffile.h>
#include <wx/tokenzr.h>
#include "ctags_manager.h"
#include "cl_editor.h"
#include "frame.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "plugin.h"

FindUsageTab::FindUsageTab(wxWindow* parent, const wxString& name)
    : OutputTabWindow(parent, wxID_ANY, name)
{
    m_styler->SetStyles(m_sci);
    m_sci->HideSelection(true);
    m_sci->Connect(wxEVT_STC_STYLENEEDED, wxStyledTextEventHandler(FindUsageTab::OnStyleNeeded), NULL, this);
    m_tb->DeleteTool(XRCID("repeat_output"));
    m_tb->Realize();
    EventNotifier::Get()->Connect(
        wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(FindUsageTab::OnThemeChanged), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &FindUsageTab::OnWorkspaceClosed, this);
}

FindUsageTab::~FindUsageTab()
{
    EventNotifier::Get()->Disconnect(
        wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(FindUsageTab::OnThemeChanged), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &FindUsageTab::OnWorkspaceClosed, this);
}

void FindUsageTab::OnStyleNeeded(wxStyledTextEvent& e)
{
    wxStyledTextCtrl* ctrl = dynamic_cast<wxStyledTextCtrl*>(e.GetEventObject());
    if(!ctrl) return;
    m_styler->StyleText(ctrl, e, true);
}

void FindUsageTab::Clear()
{
    m_matches.clear();
    m_styler->Reset();
    OutputTabWindow::Clear();
}

void FindUsageTab::OnClearAll(wxCommandEvent& e) { Clear(); }

void FindUsageTab::OnMouseDClick(wxStyledTextEvent& e)
{
    int clickedLine = wxNOT_FOUND;
    m_styler->HitTest(m_sci, e, clickedLine);

    // Did we clicked on a togglable line?
    int toggleLine = m_styler->TestToggle(m_sci, e);
    if(toggleLine != wxNOT_FOUND) {
        m_sci->ToggleFold(toggleLine);

    } else {
        UsageResultsMap::const_iterator iter = m_matches.find(clickedLine);
        if(iter != m_matches.end()) {
            DoOpenResult(iter->second);
        }
    }
}

void FindUsageTab::OnClearAllUI(wxUpdateUIEvent& e) { e.Enable(m_sci && m_sci->GetLength()); }

void FindUsageTab::ShowUsage(const CppToken::Vec_t& matches, const wxString& searchWhat)
{
    Clear();
    int lineNumber(0);
    wxString text;
    wxString curfile;
    wxString curfileContent;
    wxArrayString lines;

    text = wxString::Format(_("===== Finding references of '%s' =====\n"), searchWhat.c_str());
    lineNumber++;

    CppToken::Vec_t::const_iterator iter = matches.begin();
    for(; iter != matches.end(); ++iter) {

        // Print the line number
        wxString file_name(iter->getFilename());
        if(curfile != file_name) {
            curfile = file_name;
            wxFileName fn(file_name);
            fn.MakeRelativeTo();

            text << fn.GetFullPath() << wxT("\n");
            lineNumber++;

            // Load the file content
            wxLogNull nolog;
            wxFFile thefile(file_name, wxT("rb"));
            if(thefile.IsOpened()) {

                wxFileOffset size = thefile.Length();
                wxString fileData;
                fileData.Alloc(size);
                curfileContent.Clear();

                wxCSConv fontEncConv(wxFONTENCODING_ISO8859_1);
                thefile.ReadAll(&curfileContent, fontEncConv);

                // break the current file into lines, a line can be an empty string
                lines = wxStringTokenize(curfileContent, wxT("\n"), wxTOKEN_RET_EMPTY_ALL);
            }
        }

        // Keep the match
        m_matches[lineNumber] = *iter;

        // Format the message
        wxString linenum = wxString::Format(wxT(" %5u: "), (unsigned int)iter->getLineNumber() + 1);
        wxString scopeName(wxT("<global>"));
        TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(iter->getFilename(), iter->getLineNumber());
        if(tag) {
            scopeName = tag->GetPath();
        }

        text << linenum << wxT("[ ") << scopeName << wxT(" ] ");
        if(lines.GetCount() > iter->getLineNumber()) {
            text << lines.Item(iter->getLineNumber()).Trim().Trim(false);
        }

        text << wxT("\n");
        lineNumber++;
    }
    text << wxString::Format(_("===== Found total of %u matches =====\n"), (unsigned int)m_matches.size());
    AppendText(text);
}

void FindUsageTab::DoOpenResult(const CppToken& token)
{
    if(!token.getFilename().empty()) {
        LEditor* editor =
            clMainFrame::Get()->GetMainBook()->OpenFile(token.getFilename(), wxEmptyString, token.getLineNumber());
        if(editor) {
            editor->SetLineVisible(token.getLineNumber());
            editor->ScrollToColumn(0);
            editor->SetSelection(token.getOffset(), token.getOffset() + token.getName().length());
        }
    }
}

void FindUsageTab::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
{
    if(EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick()) {
        e.Enable(true);
        e.Check(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfReferences());

    } else {
        e.Enable(false);
        e.Check(false);
    }
}

void FindUsageTab::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    m_styler->SetStyles(m_sci);
}

void FindUsageTab::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    Clear();
}
