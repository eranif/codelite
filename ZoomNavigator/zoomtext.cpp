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

#include "ColoursAndFontsManager.h"
#include "bookmark_manager.h"
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

#include <vector>
#include <wx/app.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>

namespace
{
static constexpr int TIMER_DELAY = 150;
static constexpr int FIRST_LINE_MARKER = 1;
} // namespace

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
    EventNotifier::Get()->Bind(wxEVT_ZN_SETTINGS_UPDATED, &ZoomText::OnSettingsChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &ZoomText::OnThemeChanged, this);

    MarkerDefine(FIRST_LINE_MARKER, wxSTC_MARK_BACKGROUND, m_colour, m_colour);

    MarkerDefine(smt_warning, wxSTC_MARK_SHORTARROW);
    MarkerSetForeground(smt_error, wxColor(128, 128, 0));
    MarkerSetBackground(smt_warning, wxColor(255, 215, 0));
    MarkerSetAlpha(smt_warning, 80);

    MarkerDefine(smt_error, wxSTC_MARK_SHORTARROW);
    MarkerSetForeground(smt_error, wxColor(128, 0, 0));
    MarkerSetBackground(smt_error, wxColor(255, 0, 0));
    MarkerSetAlpha(smt_error, 80);

#ifndef __WXMSW__
    SetTwoPhaseDraw(false);
    SetBufferedDraw(false);
    SetLayoutCache(wxSTC_CACHE_DOCUMENT);
#endif

    m_timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &ZoomText::OnTimer, this, m_timer->GetId());
    Show();
}

ZoomText::~ZoomText()
{
    EventNotifier::Get()->Unbind(wxEVT_ZN_SETTINGS_UPDATED, &ZoomText::OnSettingsChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &ZoomText::OnThemeChanged, this);
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

    MarkerSetAlpha(FIRST_LINE_MARKER, 30);

    SetZoom(m_zoomFactor);
    SetEditable(false);
    SetUseHorizontalScrollBar(false);
    SetUseVerticalScrollBar(data.IsUseScrollbar());
    HideSelection(true);
    SetSTCCursor(wxSTC_CURSORARROW);
}

void ZoomText::OnSettingsChanged(wxCommandEvent& e)
{
    e.Skip();
    znConfigItem data;
    clConfig conf("zoom-navigator.conf");
    if(conf.ReadItem(&data)) {
        m_zoomFactor = data.GetZoomFactor();
        m_colour = data.GetHighlightColour();

        MarkerSetBackground(FIRST_LINE_MARKER, m_colour);
        MarkerSetAlpha(FIRST_LINE_MARKER, 50);

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

    MarkerDeleteAll(FIRST_LINE_MARKER);

    for(int i = start; i <= end; ++i) {
        MarkerAdd(i, FIRST_LINE_MARKER);
    }
}

void ZoomText::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    UpdateLexer(nullptr);
}

void ZoomText::OnTimer(wxTimerEvent& event)
{
    // sanity
    if(IsEmpty()) {
        m_timer->Start(TIMER_DELAY, true);
        return;
    }

    IEditor* editor = clGetManager()->GetActiveEditor();
    if(!editor || !editor->GetCtrl()->IsShown()) {
        m_timer->Start(TIMER_DELAY, true);
        return;
    }

    if(!editor->GetKeywordClasses().IsEmpty() && (editor->GetFileName().GetFullPath() == m_filename)) {
        // Sync between the keywords
        SetSemanticTokens(editor->GetKeywordClasses(), editor->GetKeywordLocals(), editor->GetKeywordMethods(),
                          wxEmptyString);
    }
    m_timer->Start(TIMER_DELAY, true);
}

void ZoomText::DoClear()
{
    SetReadOnly(false);
    SetText("");
    SetReadOnly(true);
}

void ZoomText::Startup() { m_timer->Start(TIMER_DELAY, true); }

void ZoomText::UpdateMarkers(const std::vector<int>& lines, MarkerType type)
{
    int marker_mask = wxNOT_FOUND;
    int marker_number = wxNOT_FOUND;
    switch(type) {
    case MARKER_ERROR:
        marker_number = smt_error;
        marker_mask = mmt_error;
        break;
    case MARKER_WARNING:
        marker_number = smt_warning;
        marker_mask = mmt_warning;
        break;
    }

    MarkerDeleteAll(marker_number);
    for(int line : lines) {
        MarkerAdd(line, marker_number);
    }
}

void ZoomText::DeleteAllMarkers()
{
    MarkerDeleteAll(smt_error);
    MarkerDeleteAll(smt_warning);
}

void ZoomText::SetSemanticTokens(const wxString& classes, const wxString& variables, const wxString& methods,
                                 const wxString& others)
{
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    wxString flatStrClasses = classes;
    wxString flatStrLocals = variables;
    wxString flatStrOthers = others;
    wxString flatStrMethods = methods;

    flatStrClasses.Trim().Trim(false);
    flatStrLocals.Trim().Trim(false);
    flatStrOthers.Trim().Trim(false);
    flatStrMethods.Trim().Trim(false);

    // locate the lexer
    auto lexer = ColoursAndFontsManager::Get().GetLexerForFile(editor->GetFileName().GetFullName());
    CHECK_PTR_RET(lexer);

    if(lexer->GetWordSet(LexerConf::WS_CLASS).is_ok()) {
        lexer->ApplyWordSet(this, LexerConf::WS_CLASS, flatStrClasses);
        lexer->ApplyWordSet(this, LexerConf::WS_FUNCTIONS, flatStrMethods);
        lexer->ApplyWordSet(this, LexerConf::WS_VARIABLES, flatStrLocals);
        lexer->ApplyWordSet(this, LexerConf::WS_OTHERS, flatStrOthers);

    } else {

        int keywords_class = wxNOT_FOUND;
        int keywords_variables = wxNOT_FOUND;

        switch(lexer->GetLexerId()) {
        case wxSTC_LEX_CPP:
            keywords_class = 1;
            keywords_variables = 3;
            break;

        case wxSTC_LEX_RUST:
            keywords_class = 3;
            keywords_variables = 4;
            break;

        case wxSTC_LEX_PYTHON:
            keywords_variables = 1;
            break;
        default:
            break;
        }
        if(!flatStrClasses.empty() && keywords_class != wxNOT_FOUND) {
            SetKeyWords(keywords_class, flatStrClasses);
        }

        if(!flatStrLocals.empty() && keywords_variables != wxNOT_FOUND) {
            SetKeyWords(keywords_variables, flatStrLocals);
        }
    }
    Colourise(0, wxSTC_INVALID_POSITION);
}
