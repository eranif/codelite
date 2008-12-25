//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : quickfindbar.cpp              
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
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/wxscintilla.h>
#include "quickfindbar.h"


BEGIN_EVENT_TABLE(QuickFindBar, wxPanel)
    EVT_BUTTON(XRCID("close_quickfind"), QuickFindBar::OnHide)
    EVT_BUTTON(XRCID("find_next_quick"), QuickFindBar::OnNext)
    EVT_BUTTON(XRCID("find_prev_quick"), QuickFindBar::OnPrev)

    EVT_TEXT(XRCID("find_what_quick"),   QuickFindBar::OnText)
    
    EVT_CHECKBOX(wxID_ANY, QuickFindBar::OnCheckBox)
    
    EVT_UPDATE_UI(XRCID("find_next_quick"), QuickFindBar::OnUpdateUI)
    EVT_UPDATE_UI(XRCID("find_prev_quick"), QuickFindBar::OnUpdateUI)
END_EVENT_TABLE()


QuickFindBar::QuickFindBar(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id)
    , m_findWhat(NULL)
    , m_sci(NULL)
    , m_flags(0)
{
    wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
    SetSizer(mainSizer);
    
    wxButton *btn = new wxBitmapButton(this, XRCID("close_quickfind"), wxXmlResource::Get()->LoadBitmap(wxT("page_close16")));
    mainSizer->Add(btn, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    wxStaticText *text = new wxStaticText(this, wxID_ANY, wxT("Find:"));
    mainSizer->Add(text, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_findWhat = new wxTextCtrl(this, XRCID("find_what_quick"));
    m_findWhat->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
    m_findWhat->SetMinSize(wxSize(200,-1));
    m_findWhat->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnKeyDown), NULL, this);
    mainSizer->Add(m_findWhat, 1, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    btn = new wxButton(this, XRCID("find_next_quick"), wxT("Next"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    mainSizer->Add(btn, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    btn = new wxButton(this, XRCID("find_prev_quick"), wxT("Prev"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    mainSizer->Add(btn, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);

    text = new wxStaticText(this, wxID_ANY, wxT("Match:"));
    mainSizer->Add(text, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    wxCheckBox *check = new wxCheckBox(this, XRCID("match_case_quick"), wxT("Case"));
    mainSizer->Add(check, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    check = new wxCheckBox(this, XRCID("match_word_quick"), wxT("Word"));
    mainSizer->Add(check, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    check = new wxCheckBox(this, XRCID("match_regexp_quick"), wxT("Regexp"));
    mainSizer->Add(check, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
     
    mainSizer->Layout();
    
    Hide();
}

bool QuickFindBar::Show(bool show)
{
    bool res = wxPanel::Show(show);
    if (res) {
        GetParent()->GetSizer()->Show(this, show, true);
        GetParent()->GetSizer()->Layout();
    }
    if (!m_sci) {
        // nothing to do
    } else if (!show) {
        m_sci->SetFocus();
    } else {
        wxString findWhat = m_sci->GetSelectedText().BeforeFirst(wxT('\n'));
        if (!findWhat.IsEmpty()) {
            m_findWhat->SetValue(m_sci->GetSelectedText().BeforeFirst(wxT('\n')));
        }
        m_findWhat->SelectAll();
        m_findWhat->SetFocus();
    }
    return res;
}

void QuickFindBar::DoSearch(bool fwd, bool incr)
{
	if (!m_sci || m_sci->GetLength() == 0 || m_findWhat->GetValue().IsEmpty()) 
		return;
	
    m_findWhat->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
    
    int start = -1, stop = -1;
    m_sci->GetSelection(&start, &stop);
    if (start != stop) {
        m_sci->SetSelection(-1, !fwd || incr ? start : stop);
    }
    m_sci->SearchAnchor();
    long pos = fwd ? m_sci->SearchNext(m_flags, m_findWhat->GetValue()) 
                   : m_sci->SearchPrev(m_flags, m_findWhat->GetValue());
    if (pos < 0) {
        // wrap around and try again
        m_sci->SetSelection(-1, fwd ? 0 : -1);
        m_sci->SearchAnchor();
        pos = fwd ? m_sci->SearchNext(m_flags, m_findWhat->GetValue()) 
                  : m_sci->SearchPrev(m_flags, m_findWhat->GetValue());
        if (pos < 0) {
            m_sci->SetSelection(start, stop);
            m_findWhat->SetBackgroundColour(wxT("PINK"));
        }
    }
    m_sci->EnsureCaretVisible();
}

void QuickFindBar::OnHide(wxCommandEvent &e)
{
	wxUnusedVar(e);
    Show(false);
}

void QuickFindBar::OnNext(wxCommandEvent &e)
{
	wxUnusedVar(e);
	DoSearch(true, false);
}

void QuickFindBar::OnPrev(wxCommandEvent &e)
{
	wxUnusedVar(e);
	DoSearch(false, false);
}

void QuickFindBar::OnText(wxCommandEvent& e)
{
	wxUnusedVar(e);
	DoSearch(true, true);
}

void QuickFindBar::OnKeyDown(wxKeyEvent& e)
{
    // simulate clicking buttons, rather than just calling the methods, in case
    // event handlers have been attached to the tools.
    wxWindow *btn = NULL;
    switch (e.GetKeyCode()) {
        case WXK_ESCAPE: 
            btn = FindWindowById(XRCID("close_quickfind"), this);
            break;
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER: 
            btn = FindWindowById(e.AltDown() ? XRCID("find_prev_quick") : XRCID("find_next_quick"), this);
            break;
    }
    if (btn) {
        wxCommandEvent cmd(wxEVT_COMMAND_BUTTON_CLICKED, btn->GetId());
        cmd.SetEventObject(btn);
        btn->ProcessEvent(cmd);
    } else {
        e.Skip();
    }
}

void QuickFindBar::OnCheckBox(wxCommandEvent &e) 
{
    int flag = 0;
    if (e.GetId() == XRCID("match_case_quick")) {
        flag = wxSCI_FIND_MATCHCASE;
    } else if (e.GetId() == XRCID("match_word_quick")) {
        flag = wxSCI_FIND_WHOLEWORD;
    } else if (e.GetId() == XRCID("match_regexp_quick")) {
        flag = wxSCI_FIND_REGEXP; // wxSCI_FIND_POSIX ??
    }
    if (e.IsChecked()) {
        m_flags |= flag;
    } else {
        m_flags &= ~flag;
    }
}

void QuickFindBar::OnUpdateUI(wxUpdateUIEvent &e)
{
    e.Enable(m_sci != NULL && m_sci->GetLength() > 0 && !m_findWhat->GetValue().IsEmpty());
}
