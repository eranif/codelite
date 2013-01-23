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

BEGIN_EVENT_TABLE(ZoomText, wxStyledTextCtrl)
    EVT_ENTER_WINDOW(ZoomText::OnEnterWindow)
    EVT_LEAVE_WINDOW(ZoomText::OnLeaveWindow)
END_EVENT_TABLE()

ZoomText::ZoomText(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxStyledTextCtrl( parent, id, pos, size, style |wxNO_BORDER, name )
    , m_enabeld(true)

{
    SetZoom( -10 );
    SetEditable( false );
    SetUseHorizontalScrollBar( false );
    SetUseVerticalScrollBar( true );
    HideSelection( true );

    SetMarginWidth(1, 0);
    SetMarginWidth(2, 0);
    SetMarginWidth(3, 0);

    znConfigItem data;
    clConfig conf("zoom-navigator.conf");
    if ( conf.ReadItem( &data ) ) {
        m_enabeld = data.IsEnabled();
    }
    MarkerDefine(1, wxSTC_MARK_BACKGROUND, wxColor(data.GetHighlightColour()), wxColor(data.GetHighlightColour()) );
    EventNotifier::Get()->Connect(wxEVT_ZN_SETTINGS_UPDATED, wxCommandEventHandler(ZoomText::OnSettingsChanged), NULL, this);
}

void ZoomText::UpdateLexer(const wxString& filename)
{
    if ( !m_enabeld )
        return;

    FileExtManager::FileType type = FileExtManager::GetType(filename);
    switch ( type ) {
    case FileExtManager::TypeHeader:
    case FileExtManager::TypeSourceC:
    case FileExtManager::TypeSourceCpp: {
        LexerConfPtr lexer = EditorConfigST::Get()->GetLexer("C++");
        lexer->Apply( this );
        SetLexer(wxSTC_LEX_CPP);
        break;
    }
    default: {
        LexerConfPtr lexer = EditorConfigST::Get()->GetLexer("Text");
        SetLexer(wxSTC_LEX_NULL);
        lexer->Apply( this );
        break;
    }
    }

    SetZoom( -8 );
    SetEditable( false );
    SetUseHorizontalScrollBar( false );
    SetUseVerticalScrollBar( true );
    HideSelection( true );
}

void ZoomText::OnSettingsChanged(wxCommandEvent &e)
{
    e.Skip();
    znConfigItem data;
    clConfig conf("zoom-navigator.conf");
    if ( conf.ReadItem( &data ) ) {
        MarkerSetBackground(1, wxColour(data.GetHighlightColour()));
        Colourise(0, wxSTC_INVALID_POSITION);
    }
}

void ZoomText::UpdateText(IEditor* editor)
{
    if ( !editor || !m_enabeld ) {
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

void ZoomText::OnEnterWindow(wxMouseEvent& e)
{
    e.Skip();
}

void ZoomText::OnLeaveWindow(wxMouseEvent& e)
{
    e.Skip();
}
