//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : syntaxhighlightdlg.cpp
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

#include "lexer_page.h"
#include <wx/xrc/xmlres.h>
#include "frame.h"
#include "windowattrmanager.h"
#include <wx/notebook.h>
#include <wx/treebook.h>
#include "macros.h"
#include "editor_config.h"
#include <wx/dir.h>
#include "syntaxhighlightdlg.h"
#include "manager.h"
#include "theme_handler.h"
#include "event_notifier.h"
#include <wx/wupdlock.h>

SyntaxHighlightDlg::SyntaxHighlightDlg( wxWindow* parent )
    : SyntaxHighlightBaseDlg( parent ), restartRequired(false)
{
    GetSizer()->Insert(0, CreateSyntaxHighlightPage(), 1, wxALL|wxEXPAND, 5);
    GetSizer()->Layout();
    GetSizer()->Fit(this);
    m_buttonOk->SetFocus();
    WindowAttrManager::Load(this, wxT("SyntaxHighlightDlgAttr"), NULL);
}

void SyntaxHighlightDlg::OnButtonOK( wxCommandEvent& event )
{
    wxUnusedVar(event);
    SaveChanges();
    clMainFrame::Get()->GetMainBook()->ApplySettingsChanges();
    // and close the dialog
    EndModal(wxID_OK);
}

void SyntaxHighlightDlg::OnButtonCancel( wxCommandEvent& event )
{
    wxUnusedVar(event);
    wxString curSelTheme = m_themes->GetStringSelection().IsEmpty() ? wxT("Default") : m_themes->GetStringSelection();
    if (curSelTheme != m_startingTheme) {
        //restore the starting theme
        EditorConfigST::Get()->SaveStringValue(wxT("LexerTheme"), m_startingTheme);
        EditorConfigST::Get()->LoadLexers(false);
    }

    EndModal(wxID_CANCEL);
}

void SyntaxHighlightDlg::OnButtonApply( wxCommandEvent& event )
{
    SaveChanges();
    clMainFrame::Get()->GetMainBook()->ApplySettingsChanges();

    m_startingTheme = m_themes->GetStringSelection().IsEmpty() ? wxT("Default") : m_themes->GetStringSelection();
    wxUnusedVar(event);
}

wxPanel *SyntaxHighlightDlg::CreateSyntaxHighlightPage()
{
    wxPanel *page = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
    page->SetSizer(sz);

    wxStaticText *txt = new wxStaticText(page, wxID_ANY, _("Colouring scheme:"), wxDefaultPosition, wxDefaultSize, 0);
    sz->Add(txt, 0, wxEXPAND|wxALL, 5);

    m_themes = new wxChoice(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, EditorConfigST::Get()->GetLexersThemes(), 0 );
    sz->Add(m_themes, 0, wxEXPAND|wxALL, 5);

    if (m_themes->IsEmpty() == false) {
        int where = m_themes->FindString(EditorConfigST::Get()->GetStringValue( wxT("LexerTheme") ));
        if ( where != wxNOT_FOUND) {
            m_themes->SetSelection( where );
        }
    }

    long style = wxNB_DEFAULT;
    m_lexersBook = new wxTreebook(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    MSWSetNativeTheme(m_lexersBook->GetTreeCtrl());
    m_lexersBook->GetTreeCtrl()->SetSizeHints(150, -1);
    sz->Add(m_lexersBook, 1, wxEXPAND | wxALL, 5);
    m_lexersBook->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    m_lexersBook->Connect(XRCID("ID_buttonTextSelApplyToAll"), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SyntaxHighlightDlg::OnTextSelApplyToAll), NULL, this );

    LoadLexers(m_themes->GetStringSelection().IsEmpty() ? wxT("Default") : m_themes->GetStringSelection());

    m_startingTheme = m_themes->GetStringSelection().IsEmpty() ? wxT("Default") : m_themes->GetStringSelection();
    ConnectChoice(m_themes, SyntaxHighlightDlg::OnThemeChanged);
    return page;
}

void SyntaxHighlightDlg::LoadLexers(const wxString& theme)
{
#ifdef __WXMSW__
    wxWindowUpdateLocker locker(this);
#endif

    // get the current open editor's lexer name
    wxString currentLexer;
    LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
    if(editor) {
        currentLexer = editor->GetContext()->GetName();
    }

    //remove old lexers
    if (m_lexersBook->GetPageCount() > 0) {
        m_lexersBook->DeleteAllPages();
    }

    //update the theme name
    EditorConfigST::Get()->SaveStringValue(wxT("LexerTheme"), theme);

    //load all lexers
    EditorConfigST::Get()->LoadLexers(false);

    std::map<wxString, LexerConfPtr>::const_iterator iter = EditorConfigST::Get()->LexerBegin();
    for (; iter != EditorConfigST::Get()->LexerEnd(); iter++) {
        LexerConfPtr lexer = iter->second;

        wxString lexName = lexer->GetName();
        lexName.Trim().Trim(false);
        if(lexName.IsEmpty())
            continue;

        // get the parent node for this lexer
        wxString firstChar = lexName.Mid(0, 1).MakeUpper();
        size_t parentIndex(wxString::npos);
        for(size_t i=0; i<m_lexersBook->GetPageCount(); i++) {
            wxString pageText = m_lexersBook->GetPageText(i);
            pageText.MakeUpper();
            if( pageText.StartsWith(firstChar) ) {
                parentIndex = i;
                break;
            }
        }

        if(parentIndex == wxString::npos) {
            // add parent node
            m_lexersBook->AddPage(CreateLexerPage(m_lexersBook, lexer), lexer->GetName(), currentLexer == iter->second->GetName());

        } else {
            m_lexersBook->InsertPage(parentIndex, CreateLexerPage(m_lexersBook, lexer), lexer->GetName(), currentLexer == iter->second->GetName());
        }
    }
    // The outputview colours are global to all a theme's lexors, so are dealt with separately
    m_colourPickerOutputPanesFgColour->SetColour(wxColour(EditorConfigST::Get()->GetCurrentOutputviewFgColour()));
    m_colourPickerOutputPanesBgColour->SetColour(wxColour(EditorConfigST::Get()->GetCurrentOutputviewBgColour()));
}

void SyntaxHighlightDlg::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    int sel = event.GetSelection();
    wxString themeName = m_themes->GetString((unsigned int)sel);

    //update the configuration with the new lexer's theme
    EditorConfigST::Get()->SaveStringValue(wxT("LexerTheme"), themeName);

    LoadLexers( themeName );
    Layout();
}

void SyntaxHighlightDlg::OnTextSelApplyToAll(wxCommandEvent& event)
{
    wxUnusedVar(event);

    LexerPage* page;
    wxWindow* win = m_lexersBook->GetCurrentPage();
    if (!win || (page = dynamic_cast<LexerPage*>(win)) == NULL) {
        return;
    }

    // Get the text-selection values for the current page, then apply them to all pages
    wxString colourstring;
    int alpha;
    page->GetTextSelectionValues(colourstring, alpha);

    for (size_t i=0; i < m_lexersBook->GetPageCount(); ++i) {
        wxWindow* win = m_lexersBook->GetPage(i);
        LexerPage* page = dynamic_cast<LexerPage*>(win);
        if (page) {
            page->SetTextSelectionValues(colourstring, alpha);
            page->SetModified();
        }
    }
}

wxPanel *SyntaxHighlightDlg::CreateLexerPage(wxWindow *parent, LexerConfPtr lexer)
{
    return new LexerPage(parent, lexer);
}

void SyntaxHighlightDlg::SaveChanges()
{
    int max = m_lexersBook->GetPageCount();
    for (int i=0; i<max; i++) {
        wxWindow *win = m_lexersBook->GetPage((size_t)i);
        LexerPage *page = dynamic_cast<LexerPage*>( win );
        if ( page ) {
            page->SaveSettings();
        }
    }

    // Save all lexers once
    EditorConfigST::Get()->SaveLexers();

    wxString oldFg = EditorConfigST::Get()->GetCurrentOutputviewFgColour();
    wxString oldBg = EditorConfigST::Get()->GetCurrentOutputviewBgColour();
    
    wxString newFg = m_colourPickerOutputPanesFgColour->GetColour().GetAsString(wxC2S_HTML_SYNTAX);
    EditorConfigST::Get()->SetCurrentOutputviewFgColour(newFg);
    
    wxString newBg = m_colourPickerOutputPanesBgColour->GetColour().GetAsString(wxC2S_HTML_SYNTAX);
    EditorConfigST::Get()->SetCurrentOutputviewBgColour(newBg);
    
    // Check for conditions for sending the THEME_CHANGED event
    wxString curSelTheme = m_themes->GetStringSelection().IsEmpty() ? wxT("Default") : m_themes->GetStringSelection();
    
    if ( (oldBg != newBg) || (oldFg != newFg) || (m_startingTheme != curSelTheme) ) {
        wxCommandEvent evtThemeChanged(wxEVT_CL_THEME_CHANGED);
        EventNotifier::Get()->AddPendingEvent(evtThemeChanged);
    }
}

SyntaxHighlightDlg::~SyntaxHighlightDlg()
{
    WindowAttrManager::Save(this, wxT("SyntaxHighlightDlgAttr"), NULL);
}

void SyntaxHighlightDlg::OnRestoreDefaults(wxCommandEvent& e)
{
    if(wxMessageBox(_("Are you sure you want to load all default syntax highlight settings and lose all your changes?"), _("CodeLite"), wxYES_NO|wxCANCEL|wxICON_QUESTION|wxCENTER, this) != wxYES) {
        return;
    }

    // restore the default lexers
    EditorConfigST::Get()->LoadLexers(true);
    clMainFrame::Get()->GetMainBook()->ApplySettingsChanges();

    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("syntax_highlight"));
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(event);
    EndModal(wxID_OK);
}
