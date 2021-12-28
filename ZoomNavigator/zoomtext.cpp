//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : zoomtext.cpp
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

//	Copyright: 2013 Brandon Captain
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.

#include "zoomtext.h"

#include "cl_config.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "fileextmanager.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "plugin.h"
#include "znSettingsDlg.h"
#include "zn_config_item.h"

#include <wx/app.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>

ZoomText::ZoomText(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                   const wxString& name)
{
    Hide();
    if(!wxStyledTextCtrl::Create(parent, id, pos, size, style | wxNO_BORDER, name)) {
        return;
    }

    wxColour bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX);
    for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
        StyleSetBackground(i, bgColour);
    }

    znConfigItem data;
    clConfig conf("zoom-navigator.conf");
    conf.ReadItem(&data);

    SetEditable(false);
    SetUseHorizontalScrollBar(false);
    SetUseVerticalScrollBar(data.IsUseScrollbar());
    HideSelection(true);

    SetMarginWidth(1, 0);
    SetMarginWidth(2, 0);
    SetMarginWidth(3, 0);

    m_zoomFactor = data.GetZoomFactor();
    m_colour = data.GetHighlightColour();
    MarkerSetBackground(1, m_colour);
    SetZoom(m_zoomFactor);
    EventNotifier::Get()->Connect(wxEVT_ZN_SETTINGS_UPDATED, wxCommandEventHandler(ZoomText::OnSettingsChanged), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(ZoomText::OnThemeChanged), NULL, this);
    MarkerDefine(1, wxSTC_MARK_BACKGROUND, m_colour, m_colour);

#ifndef __WXMSW__
    SetTwoPhaseDraw(false);
    SetBufferedDraw(false);
    SetLayoutCache(wxSTC_CACHE_DOCUMENT);
#endif
    MarkerSetAlpha(1, 10);

    m_timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &ZoomText::OnTimer, this, m_timer->GetId());
    Show();
}

ZoomText::~ZoomText()
{
    EventNotifier::Get()->Disconnect(wxEVT_ZN_SETTINGS_UPDATED, wxCommandEventHandler(ZoomText::OnSettingsChanged),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(ZoomText::OnThemeChanged), NULL,
                                     this);
    Unbind(wxEVT_TIMER, &ZoomText::OnTimer, this, m_timer->GetId());

    if(m_timer->IsRunning()) {
        m_timer->Stop();
    }
    wxDELETE(m_timer);
}

void ZoomText::UpdateLexer(IEditor* editor)
{
    if(!editor) {
        editor = clGetManager()->GetActiveEditor();
    }
    if(!editor) {
        DoClear();
        return;
    }

    znConfigItem data;
    clConfig conf("zoom-navigator.conf");
    conf.ReadItem(&data);

    m_filename = editor->GetFileName().GetFullPath();
    LexerConf::Ptr_t lexer = EditorConfigST::Get()->GetLexerForFile(m_filename);
    if(!lexer) {
        lexer = EditorConfigST::Get()->GetLexer("Text");
    }
    lexer->Apply(this, true);

    if(lexer->IsDark()) {
        MarkerSetAlpha(1, 10);
    } else {
        MarkerSetAlpha(1, 20);
    }

    SetZoom(m_zoomFactor);
    SetEditable(false);
    SetUseHorizontalScrollBar(false);
    SetUseVerticalScrollBar(data.IsUseScrollbar());
    HideSelection(true);
    MarkerSetBackground(1, m_colour);
}

void ZoomText::OnSettingsChanged(wxCommandEvent& e)
{
    e.Skip();
    znConfigItem data;
    clConfig conf("zoom-navigator.conf");
    if(conf.ReadItem(&data)) {
        m_zoomFactor = data.GetZoomFactor();
        m_colour = data.GetHighlightColour();
        MarkerSetBackground(1, m_colour);
        SetZoom(m_zoomFactor);
        Colourise(0, wxSTC_INVALID_POSITION);
    }
}

void ZoomText::UpdateText(IEditor* editor)
{
    if(!editor) {
        DoClear();

    } else {
        SetReadOnly(false);
        SetText(editor->GetEditorText());
        SetReadOnly(true);
        SetCurrentPos(editor->GetCurrentPosition());
    }
}

void ZoomText::HighlightLines(int start, int end)
{
    int nLineCount = end - start;
    int lastLine = LineFromPosition(GetLength());
    if(lastLine < end) {
        end = lastLine;
        start = end - nLineCount;
        if(start < 0)
            start = 0;
    }

    MarkerDeleteAll(1);
    for(int i = start; i <= end; ++i) {
        MarkerAdd(i, 1);
    }
}

void ZoomText::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    UpdateLexer(NULL);
}

void ZoomText::OnTimer(wxTimerEvent& event)
{
    // sanity
    if(!m_classes.IsEmpty() || IsEmpty()) {
        m_timer->Start(1000, true);
        return;
    }

    IEditor* editor = clGetManager()->GetActiveEditor();
    if(!editor) {
        m_timer->Start(1000, true);
        return;
    }

    if(m_classes.IsEmpty() && !editor->GetKeywordClasses().IsEmpty() &&
       (editor->GetFileName().GetFullPath() == m_filename)) {
        // Sync between the keywords
        SetKeyWords(1, editor->GetKeywordClasses()); // classes
        SetKeyWords(3, editor->GetKeywordLocals());  // locals
        Colourise(0, GetLength());
    }
    m_timer->Start(1000, true);
}

void ZoomText::DoClear()
{
    m_classes.clear();
    m_locals.clear();
    SetReadOnly(false);
    SetText("");
    SetReadOnly(true);
}

void ZoomText::Startup() { m_timer->Start(1000, true); }
