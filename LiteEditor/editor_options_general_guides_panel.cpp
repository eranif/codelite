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
    EnableDisableCaretLineColourPicker();

	switch (options->GetShowWhitspaces()) {
	case wxSCI_WS_VISIBLEALWAYS:
		m_whitespaceStyle->SetStringSelection(wxT("Visible always"));
		break;
	case wxSCI_WS_VISIBLEAFTERINDENT:
		m_whitespaceStyle->SetStringSelection(wxT("Visible after indent"));
		break;
    case wxSCI_WS_INDENTVISIBLE:
        m_whitespaceStyle->SetStringSelection(wxT("Indentation only"));
        break;
	default:
		m_whitespaceStyle->SetStringSelection(wxT("Invisible"));
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

	// save the whitespace visibility
	int style(wxSCI_WS_INVISIBLE); // inivisble
	if (m_whitespaceStyle->GetStringSelection() == wxT("Visible always")) {
		style = wxSCI_WS_VISIBLEALWAYS;
	} else if (m_whitespaceStyle->GetStringSelection() == wxT("Visible after indent")) {
		style = wxSCI_WS_VISIBLEAFTERINDENT;
	} else if (m_whitespaceStyle->GetStringSelection() == wxT("Indentation only")) {
        style = wxSCI_WS_INDENTVISIBLE;
    }
	options->SetShowWhitspaces(style);
}

void EditorOptionsGeneralGuidesPanel::EnableDisableCaretLineColourPicker()
{
    if (m_highlightCaretLine->IsChecked()) {
        m_staticText1->Enable();
        m_caretLineColourPicker->Enable();
    } else {
        m_staticText1->Disable();
        m_caretLineColourPicker->Disable();
    }
}

void EditorOptionsGeneralGuidesPanel::OnHighlightCaretLine(wxCommandEvent& e)
{
    EnableDisableCaretLineColourPicker();
}
