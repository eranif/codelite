//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svnblameeditor.cpp
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

#include "svnblameeditor.h"
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include "drawingutils.h"
#include <map>
#include <wx/tokenzr.h>
#include <wx/font.h>
#include <wx/settings.h>

BEGIN_EVENT_TABLE(SvnBlameEditor, wxStyledTextCtrl)
    EVT_CONTEXT_MENU(SvnBlameEditor::OnContextMenu)
END_EVENT_TABLE()

#define MARGIN_STYLE_START 48
#define MARGIN_FIRST_STYLE 49

SvnBlameEditor::SvnBlameEditor(wxWindow *parent, wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size, long style,
                               const wxString& name)
    : wxStyledTextCtrl(parent, id, pos, size, style, name)
{
    Initialize();
}

void SvnBlameEditor::Initialize()
{
    // Initialize some styles
    SetMarginType (0, wxSTC_MARGIN_TEXT  );
    SetMarginType (1, wxSTC_MARGIN_NUMBER);
    SetMarginWidth(1, 4 + 5*TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9")));
    SetMarginWidth(2, 0);
    SetMarginWidth(3, 0);
    SetMarginWidth(4, 0);
    SetTabWidth(4);
    
    // Define some colors to use
    StyleSetBackground(MARGIN_STYLE_START + 1,  DrawingUtils::LightColour(wxT("GREEN"),      7.0));
    StyleSetBackground(MARGIN_STYLE_START + 2,  DrawingUtils::LightColour(wxT("BLUE"),       7.0));
    StyleSetBackground(MARGIN_STYLE_START + 3,  DrawingUtils::LightColour(wxT("ORANGE"),     7.0));
    StyleSetBackground(MARGIN_STYLE_START + 4,  DrawingUtils::LightColour(wxT("YELLOW"),     7.0));
    StyleSetBackground(MARGIN_STYLE_START + 5,  DrawingUtils::LightColour(wxT("PURPLE"),     7.0));
    StyleSetBackground(MARGIN_STYLE_START + 6,  DrawingUtils::LightColour(wxT("RED"),        7.0));
    StyleSetBackground(MARGIN_STYLE_START + 7,  DrawingUtils::LightColour(wxT("BROWN"),      7.0));
    StyleSetBackground(MARGIN_STYLE_START + 8,  DrawingUtils::LightColour(wxT("LIGHT GREY"), 7.0));
    StyleSetBackground(MARGIN_STYLE_START + 9,  DrawingUtils::LightColour(wxT("SIENNA"),     7.0));

    StyleSetBackground(MARGIN_STYLE_START + 10, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    StyleSetForeground(MARGIN_STYLE_START + 10, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
}

void SvnBlameEditor::SetText(const wxString& text)
{
    // Define some colors
    int xx, yy;
    int marginWidth(0);

    int s_style(MARGIN_FIRST_STYLE);
    std::map<wxString, int> authorsColorsMap;

    wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    wxArrayString lines = wxStringTokenize(text, wxT("\n"), wxTOKEN_RET_DELIMS);
    for (size_t i=0; i<lines.GetCount(); i++) {
        wxString revision;
        wxString author;
        wxString text;

        wxString line = lines.Item(i);
        line.Trim(false);

        revision = line.BeforeFirst(wxT(' '));
        revision.Trim().Trim(false);
        line = line.AfterFirst(wxT(' '));
        line.Trim(false);

        author = line.BeforeFirst(wxT(' '));
        author.Trim().Trim(false);

        int style;
        std::map<wxString, int>::iterator iter = authorsColorsMap.find(author);
        if (iter != authorsColorsMap.end()) {
            // Create new random color and use it
            style = (*iter).second;
        } else {
            style = s_style;
            s_style ++;
            if (s_style > (MARGIN_STYLE_START+9) )
                s_style = MARGIN_FIRST_STYLE;
            authorsColorsMap[author] = style;
        }

        line = line.AfterFirst(wxT(' '));

        wxString marginText = wxString::Format(wxT("% 8s: %s"), revision.c_str(), author.c_str());
        wxWindow::GetTextExtent(marginText, &xx, &yy, NULL, NULL, &font);

        marginWidth = wxMax(marginWidth, xx);
        AppendText(line);
        MarginSetText((int)i, marginText);
        MarginSetStyle((int)i, style);

        // Keep the revision on in array
        BlameLineInfo info;
        info.revision = revision;
        info.style    = style;
        m_lineInfo.push_back(info);
    }

    SetMarginWidth(0, marginWidth);
    SetReadOnly(true);
}

void SvnBlameEditor::OnContextMenu(wxContextMenuEvent& event)
{
    wxPoint pt       = event.GetPosition();
    wxPoint clientPt = ScreenToClient(pt);

    int margin = GetMarginWidth(0); // get the margin width
    if ( clientPt.x < margin ) {

        GotoPos( PositionFromPoint(clientPt) );
        // Margin context menu
        wxMenu menu;
        menu.Append( XRCID("svn_highlight_revision"), _("Highlight this revision"), _("Highlight this revision"), false);
        menu.Connect(XRCID("svn_highlight_revision"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SvnBlameEditor::OnHighlightRevision), NULL, this);

        PopupMenu(&menu);

    } else {
        wxStyledTextCtrl::OnContextMenu(event);

    }
}

void SvnBlameEditor::OnHighlightRevision(wxCommandEvent& event)
{
    int line = GetCurrentLine();
    if(line < (int)m_lineInfo.size() && line >= 0) {

        BlameLineInfo info  = m_lineInfo.at(line);
        wxString  revision  = info.revision;

        // Loop over the lines and adjust the styles
        for(size_t i=0; i<m_lineInfo.size(); i++) {
            BlameLineInfo lineInfo = m_lineInfo.at(i);
            if(lineInfo.revision == revision) {
                // Highlight this line
                MarginSetStyle((int)i, 10);

            } else {
                // restore old settings
                MarginSetStyle((int)i, lineInfo.style);
            }
        }
        Colourise(0, -1);
    }
}
