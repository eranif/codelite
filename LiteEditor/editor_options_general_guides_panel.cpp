//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editor_options_general_guides_panel.cpp
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

#include <wx/wxscintilla.h>
#include "editor_options_general_guides_panel.h"

EditorOptionsGeneralGuidesPanel::EditorOptionsGeneralGuidesPanel( wxWindow* parent )
		: EditorOptionsGeneralGuidesPanelBase( parent )
		, TreeBookNode<EditorOptionsGeneralGuidesPanel>()
{
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();

	m_displayLineNumbers->SetValue(options->GetDisplayLineNumbers());
	m_checkBoxMatchBraces->SetValue(options->GetHighlightMatchedBraces());
	m_showIndentationGuideLines->SetValue(options->GetShowIndentationGuidelines());
	m_checkBoxAutoCompleteBraces->SetValue(options->GetAutoAddMatchedBraces());
	
	m_highlightCaretLine->SetValue(options->GetHighlightCaretLine());
	m_caretLineColourPicker->SetColour(options->GetCaretLineColour());
	m_choiceEOL->SetStringSelection(options->GetEolMode());
	m_checkBoxHideChangeMarkerMargin->SetValue( options->GetHideChangeMarkerMargin() );
	m_checkBoxDisableSemicolonShift->SetValue( options->GetDisableSemicolonShift() );
	m_sliderCaretLineAlpha->SetValue(options->GetCaretLineAlpha());
	
	switch (options->GetShowWhitspaces()) {
	case wxSCI_WS_VISIBLEALWAYS:
		m_whitespaceStyle->SetStringSelection(_("Visible always"));
		break;
	case wxSCI_WS_VISIBLEAFTERINDENT:
		m_whitespaceStyle->SetStringSelection(_("Visible after indentation"));
		break;
	case wxSCI_WS_INDENTVISIBLE:
		m_whitespaceStyle->SetStringSelection(_("Indentation only"));
		break;
	default:
		m_whitespaceStyle->SetStringSelection(_("Invisible"));
		break;
	}

}


void EditorOptionsGeneralGuidesPanel::Save(OptionsConfigPtr options)
{
	options->SetDisplayLineNumbers( m_displayLineNumbers->IsChecked() );
	options->SetHighlightMatchedBraces(m_checkBoxMatchBraces->IsChecked());
	options->SetShowIndentationGuidelines( m_showIndentationGuideLines->IsChecked() );
	options->SetHighlightCaretLine( m_highlightCaretLine->IsChecked() );
	options->SetCaretLineColour(m_caretLineColourPicker->GetColour());
	options->SetAutoAddMatchedBraces(m_checkBoxAutoCompleteBraces->IsChecked());
	options->SetEolMode(m_choiceEOL->GetStringSelection());
	options->SetHideChangeMarkerMargin( m_checkBoxHideChangeMarkerMargin->IsChecked() );
	options->SetDisableSemicolonShift( m_checkBoxDisableSemicolonShift->IsChecked() );
	options->SetCaretLineAlpha(m_sliderCaretLineAlpha->GetValue());
	
	// save the whitespace visibility
	int style(wxSCI_WS_INVISIBLE); // inivisble
	if (m_whitespaceStyle->GetStringSelection() == _("Visible always")) {
		style = wxSCI_WS_VISIBLEALWAYS;
	} else if (m_whitespaceStyle->GetStringSelection() == _("Visible after indentation")) {
		style = wxSCI_WS_VISIBLEAFTERINDENT;
	} else if (m_whitespaceStyle->GetStringSelection() == _("Indentation only")) {
		style = wxSCI_WS_INDENTVISIBLE;
	}
	options->SetShowWhitspaces(style);
}

void EditorOptionsGeneralGuidesPanel::OnhighlightCaretLineUI(wxUpdateUIEvent& event)
{
	event.Enable(m_highlightCaretLine->IsChecked());
}
