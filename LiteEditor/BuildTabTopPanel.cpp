//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : BuildTabTopPanel.cpp
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

#include "BuildTabTopPanel.h"
#include "bitmap_loader.h"
#include "cl_config.h"
#include "globals.h"
#include <imanager.h>

#include "editor_config.h"
#include "frame.h"
#include "globals.h"
#include "mainbook.h"
#include "new_build_tab.h"
#include <wx/ffile.h>
#include <wx/filename.h>

BuildTabTopPanel::BuildTabTopPanel(wxWindow* parent)
    : wxPanel(parent)
{
    m_buildTab = dynamic_cast<NewBuildTab*>(parent);
    BitmapLoader* bmps = clGetManager()->GetStdIcons();
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_toolbar = new clToolBar(this, wxID_ANY);
    m_toolbar->AddTool(XRCID("build_tool_pin"), _("Pin build view"), bmps->LoadBitmap("ToolPin"), _("Pin build view"),
                       wxITEM_CHECK);
    m_toolbar->AddTool(XRCID("build_tool_scroll_view"), _("Scroll to last line"), bmps->LoadBitmap("link_editor"),
                       _("Scroll to last line"), wxITEM_CHECK);
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("build_tool_clear"), _("Clear view"), bmps->LoadBitmap("clear"), _("Clear view"));
    m_toolbar->AddTool(XRCID("build_tool_save"), _("Save build log"), bmps->LoadBitmap("file_save"),
                       _("Save build log"));
    m_toolbar->AddTool(XRCID("build_tool_copy"), _("Copy build log"), bmps->LoadBitmap("copy"), _("Copy build log"));
    m_toolbar->AddTool(XRCID("build_tool_paste"), _("Paste build log"), bmps->LoadBitmap("paste"),
                       _("Paste build log"));
    m_toolbar->Realize();
    GetSizer()->Add(m_toolbar, 0, wxEXPAND);

    m_toolbar->Bind(wxEVT_MENU, &BuildTabTopPanel::OnToolPinCommandToolClicked, this, XRCID("build_tool_pin"));
    m_toolbar->Bind(wxEVT_MENU, &BuildTabTopPanel::OnAutoScroll, this, XRCID("build_tool_scroll_view"));
    m_toolbar->Bind(wxEVT_MENU, &BuildTabTopPanel::OnClearBuildOutput, this, XRCID("build_tool_clear"));
    m_toolbar->Bind(wxEVT_MENU, &BuildTabTopPanel::OnSaveBuildOutput, this, XRCID("build_tool_save"));
    m_toolbar->Bind(wxEVT_MENU, &BuildTabTopPanel::OnCopyBuildOutput, this, XRCID("build_tool_copy"));
    m_toolbar->Bind(wxEVT_MENU, &BuildTabTopPanel::OnPaste, this, XRCID("build_tool_paste"));

    m_toolbar->Bind(wxEVT_UPDATE_UI, &BuildTabTopPanel::OnAutoScrollUI, this, XRCID("build_tool_scroll_view"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &BuildTabTopPanel::OnClearBuildOutputUI, this, XRCID("build_tool_clear"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &BuildTabTopPanel::OnSaveBuildOutputUI, this, XRCID("build_tool_save"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &BuildTabTopPanel::OnCopyBuildOutputUI, this, XRCID("build_tool_copy"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &BuildTabTopPanel::OnPasteUI, this, XRCID("build_tool_paste"));
}

BuildTabTopPanel::~BuildTabTopPanel() {}

void BuildTabTopPanel::OnToolPinCommandToolClicked(wxCommandEvent& event)
{
    EditorConfigST::Get()->SetPaneStickiness("Build", event.IsChecked());
}

void BuildTabTopPanel::OnClearBuildOutput(wxCommandEvent& WXUNUSED(event))
{
    if(m_buildTab) { m_buildTab->Clear(); }
}

void BuildTabTopPanel::OnClearBuildOutputUI(wxUpdateUIEvent& event)
{
    if(!m_buildTab) {
        event.Enable(false);

    } else {
        event.Enable(!m_buildTab->IsEmpty() && !m_buildTab->IsBuildInProgress());
    }
}

void BuildTabTopPanel::OnSaveBuildOutput(wxCommandEvent& WXUNUSED(event))
{
    wxString filename = ::wxFileSelector(_("Select a file"), wxEmptyString, wxT("BuildLog.txt"), wxEmptyString,
                                         wxFileSelectorDefaultWildcardStr, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(filename.IsEmpty()) return;

    wxFileName fn(filename);
    wxFFile fp(fn.GetFullPath(), wxT("w+b"));
    if(fp.IsOpened()) {
        fp.Write(m_buildTab->GetBuildContent(), wxConvUTF8);
        fp.Close();

        ::wxMessageBox(_("Saved build log to file:\n") + fn.GetFullPath());
    }
}

void BuildTabTopPanel::OnSaveBuildOutputUI(wxUpdateUIEvent& event)
{
    if(!m_buildTab) {
        event.Enable(false);

    } else {
        event.Enable(!m_buildTab->IsEmpty() && !m_buildTab->IsBuildInProgress());
    }
}
void BuildTabTopPanel::OnCopyBuildOutput(wxCommandEvent& event)
{
    wxString content = m_buildTab->GetBuildContent();
    ::CopyToClipboard(content);
}

void BuildTabTopPanel::OnCopyBuildOutputUI(wxUpdateUIEvent& event)
{
    if(!m_buildTab) {
        event.Enable(false);

    } else {
        event.Enable(!m_buildTab->IsEmpty());
    }
}

void BuildTabTopPanel::OnPaste(wxCommandEvent& event)
{
    wxString content = m_buildTab->GetBuildContent();
    clEditor* editor = clMainFrame::Get()->GetMainBook()->NewEditor();
    if(editor) { editor->SetText(content); }
}

void BuildTabTopPanel::OnPasteUI(wxUpdateUIEvent& event)
{
    if(!m_buildTab) {
        event.Enable(false);

    } else {
        event.Enable(!m_buildTab->IsEmpty());
    }
}
void BuildTabTopPanel::OnAutoScroll(wxCommandEvent& event)
{
    clConfig::Get().Write(kConfigBuildAutoScroll, event.IsChecked());
    if(event.IsChecked()) { m_buildTab->ScrollToBottom(); }
}

void BuildTabTopPanel::OnAutoScrollUI(wxUpdateUIEvent& event)
{
    event.Check(clConfig::Get().Read(kConfigBuildAutoScroll, true));
}
