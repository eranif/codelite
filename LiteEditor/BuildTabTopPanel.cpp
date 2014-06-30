//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
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
#include "cl_config.h"

#if CL_USE_NEW_BUILD_TAB

#include "new_build_tab.h"
#include "editor_config.h"
#include <wx/ffile.h>
#include <wx/filename.h>
#include "globals.h"
#include "frame.h"
#include "mainbook.h"

BuildTabTopPanel::BuildTabTopPanel(wxWindow* parent)
    : BuildTabTopPanelBaseClass(parent)
{
    m_buildTab = dynamic_cast<NewBuildTab*>(parent);
}

BuildTabTopPanel::~BuildTabTopPanel()
{
}

void BuildTabTopPanel::OnToolPinCommandToolClicked(wxCommandEvent& event)
{
    EditorConfigST::Get()->SetPaneStickiness("Build", event.IsChecked());
}


void BuildTabTopPanel::OnClearBuildOutput(wxCommandEvent& WXUNUSED(event))
{
    if ( m_buildTab ) {
        m_buildTab->Clear();
    }
}

void BuildTabTopPanel::OnClearBuildOutputUI(wxUpdateUIEvent& event)
{
    if ( !m_buildTab ) {
        event.Enable(false);

    } else {
        event.Enable( !m_buildTab->IsEmpty() && !m_buildTab->IsBuildInProgress() );

    }
}

void BuildTabTopPanel::OnSaveBuildOutput(wxCommandEvent& WXUNUSED(event))
{
    wxString filename = ::wxFileSelector(_("Select a file"), wxEmptyString, wxT("BuildLog.txt"), wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if(filename.IsEmpty())
        return;

    wxFileName fn(filename);
    wxFFile fp(fn.GetFullPath(), wxT("w+b"));
    if( fp.IsOpened() ) {
        fp.Write(m_buildTab->GetBuildContent(), wxConvUTF8);
        fp.Close();

        ::wxMessageBox(_("Saved build log to file:\n") + fn.GetFullPath());
    }
}

void BuildTabTopPanel::OnSaveBuildOutputUI(wxUpdateUIEvent& event)
{
    if ( !m_buildTab ) {
        event.Enable(false);

    } else {
        event.Enable( !m_buildTab->IsEmpty() && !m_buildTab->IsBuildInProgress() );

    }
}
void BuildTabTopPanel::OnCopyBuildOutput(wxCommandEvent& event)
{
    wxString content = m_buildTab->GetBuildContent();
    ::CopyToClipboard(content);
}

void BuildTabTopPanel::OnCopyBuildOutputUI(wxUpdateUIEvent& event)
{
    if ( !m_buildTab ) {
        event.Enable(false);

    } else {
        event.Enable( !m_buildTab->IsEmpty() );

    }
}

void BuildTabTopPanel::OnPaste(wxCommandEvent& event)
{
    wxString content = m_buildTab->GetBuildContent();
    LEditor *editor = clMainFrame::Get()->GetMainBook()->NewEditor();
    if ( editor ) {
        editor->SetText(content);
    }
}

void BuildTabTopPanel::OnPasteUI(wxUpdateUIEvent& event)
{
    if ( !m_buildTab ) {
        event.Enable(false);

    } else {
        event.Enable( !m_buildTab->IsEmpty() );

    }
}
void BuildTabTopPanel::OnAutoScroll(wxCommandEvent& event)
{
    clConfig::Get().Write("build-auto-scroll", event.IsChecked());
    if ( event.IsChecked() ) {
        m_buildTab->ScrollToBottom();
    }
}

void BuildTabTopPanel::OnAutoScrollUI(wxUpdateUIEvent& event)
{
    event.Check( clConfig::Get().Read("build-auto-scroll", true) );
}

#endif
