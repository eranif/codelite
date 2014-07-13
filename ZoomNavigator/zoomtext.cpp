//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
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

#include <wx/xrc/xmlres.h>
#include "plugin.h"
#include "zoomtext.h"
#include "fileextmanager.h"
#include "lexer_configuration.h"
#include "editor_config.h"
#include "zn_config_item.h"
#include "cl_config.h"
#include "znSettingsDlg.h"
#include "event_notifier.h"
#include "plugin.h"

ZoomText::ZoomText(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxStyledTextCtrl( parent, id, pos, size, style |wxNO_BORDER, name )
{
    SetEditable( false );
    SetUseHorizontalScrollBar( false );
    SetUseVerticalScrollBar( true );
    HideSelection( true );

    SetMarginWidth(1, 0);
    SetMarginWidth(2, 0);
    SetMarginWidth(3, 0);

    znConfigItem data;
    clConfig conf("zoom-navigator.conf");
    conf.ReadItem( &data );
    
    m_zoomFactor = data.GetZoomFactor();
    m_colour = data.GetHighlightColour();
    MarkerSetBackground(1, m_colour);
    SetZoom( m_zoomFactor );
    EventNotifier::Get()->Connect(wxEVT_ZN_SETTINGS_UPDATED, wxCommandEventHandler(ZoomText::OnSettingsChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(ZoomText::OnThemeChanged), NULL, this);
    MarkerDefine(1, wxSTC_MARK_BACKGROUND, m_colour, m_colour );

#ifndef __WXMSW__    
    SetTwoPhaseDraw(false);
    SetBufferedDraw(false);
    SetLayoutCache(wxSTC_CACHE_DOCUMENT);
#endif

#ifdef __WXMSW__    
    MarkerSetAlpha(1, 50);
#endif    
}

ZoomText::~ZoomText()
{
    EventNotifier::Get()->Disconnect(wxEVT_ZN_SETTINGS_UPDATED, wxCommandEventHandler(ZoomText::OnSettingsChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(ZoomText::OnThemeChanged), NULL, this);
}

void ZoomText::UpdateLexer(const wxString& filename)
{
    m_filename = filename;
    LexerConf::Ptr_t lexer = EditorConfigST::Get()->GetLexerForFile(filename);
    if ( !lexer ) {
        lexer = EditorConfigST::Get()->GetLexer("Text");
    }
    lexer->Apply( this );
    
    SetZoom( m_zoomFactor );
    SetEditable( false );
    SetUseHorizontalScrollBar( false );
    SetUseVerticalScrollBar( true );
    HideSelection( true );
    MarkerSetBackground(1, m_colour);
}

void ZoomText::OnSettingsChanged(wxCommandEvent &e)
{
    e.Skip();
    znConfigItem data;
    clConfig conf("zoom-navigator.conf");
    if ( conf.ReadItem( &data ) ) {
        m_zoomFactor = data.GetZoomFactor();
        m_colour = data.GetHighlightColour();
        MarkerSetBackground(1, m_colour);
        SetZoom(m_zoomFactor);
        Colourise(0, wxSTC_INVALID_POSITION);
    }
}

void ZoomText::UpdateText(IEditor* editor)
{
    if ( !editor ) {
        SetReadOnly( false );
        SetText( "" );
        SetReadOnly( true );

    } else {
        SetReadOnly( false );
        SetText( editor->GetEditorText() );
        SetReadOnly( true );
        SetCurrentPos( editor->GetCurrentPosition() );
    }
}

void ZoomText::HighlightLines(int start, int end)
{
    int nLineCount = end - start;
    int lastLine = LineFromPosition(GetLength());
    if ( lastLine < end ) {
        end = lastLine;
        start = end - nLineCount;
        if ( start < 0 )
            start = 0;
    }
        
    MarkerDeleteAll(1);
    for(int i=start; i<=end; ++i) {
        MarkerAdd(i, 1);
    }
}

void ZoomText::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    UpdateLexer(m_filename);
}
