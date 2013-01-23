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

ZoomText::ZoomText(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
 : wxStyledTextCtrl( parent, id, pos, size, style, name )
{ 
    SetZoom( -8 );
    SetEditable( false );
    SetUseHorizontalScrollBar( false );
    SetUseVerticalScrollBar( false );
    MarkerDefine(1, wxSTC_MARK_BACKGROUND, wxColor(wxT("LIGHT GREEN")), wxColor(wxT("LIGHT GREEN")) );
    HideSelection( true );
}

void ZoomText::UpdateLexer(const wxString& filename)
{
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
}
