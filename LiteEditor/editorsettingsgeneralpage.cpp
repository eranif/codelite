#include <wx/wxscintilla.h>
#include "editor_config.h"
#include "editorsettingsgeneralpage.h"

EditorSettingsGeneralPage::EditorSettingsGeneralPage( wxWindow* parent )
		: EditorSettingsGeneralPageBase( parent )
{
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	m_displayLineNumbers->SetValue(options->GetDisplayLineNumbers());
	m_indentsUsesTabs->SetValue(options->GetIndentUsesTabs());
	m_showIndentationGuideLines->SetValue(options->GetShowIndentationGuidelines());
	m_highlighyCaretLine->SetValue(options->GetHighlightCaretLine());
	m_caretLineColourPicker->SetColour(options->GetCaretLineColour());

	switch (options->GetShowWhitspaces()) {
	case 0:
		m_whitespaceStyle->SetStringSelection(wxT("Invisible"));
		break;
	case 1:
		m_whitespaceStyle->SetStringSelection(wxT("Visible always"));
		break;
	case 2:
		m_whitespaceStyle->SetStringSelection(wxT("Visible after indent"));
		break;
	default:
		m_whitespaceStyle->SetStringSelection(wxT("Invisible"));
		break;
	}

	long value(4);
	EditorConfigST::Get()->GetLongValue(wxT("EditorTabWidth"), value);
	m_spinCtrlTabWidth->SetValue(value);

	value = 0;
	EditorConfigST::Get()->GetLongValue(wxT("QuickCodeNavigationUsesMouseMiddleButton"), value);
	m_radioBoxNavigationMethod->SetSelection(0);
	if(value == 1){
		m_radioBoxNavigationMethod->SetSelection(1);
	}

	long trim(0);
	EditorConfigST::Get()->GetLongValue(wxT("EditorTrimEmptyLines"), trim);
	m_checkBoxTrimLine->SetValue(trim ? true : false);

	long appendLf(0);
	EditorConfigST::Get()->GetLongValue(wxT("EditorAppendLf"), appendLf);
	m_checkBoxAppendLF->SetValue(appendLf ? true : false);

    m_radioBtnRMDisabled->SetValue(options->GetEdgeMode() == wxSCI_EDGE_NONE);
    m_radioBtnRMLine->SetValue(options->GetEdgeMode() == wxSCI_EDGE_LINE);
    m_radioBtnRMBackground->SetValue(options->GetEdgeMode() == wxSCI_EDGE_BACKGROUND);
    m_rightMarginColumn->SetValue(options->GetEdgeColumn());
    m_rightMarginColour->SetColour(options->GetEdgeColour());
	m_checkBoxMatchBraces->SetValue(options->GetHighlightMatchedBraces());
	EnableDisableRightMargin();
}

void EditorSettingsGeneralPage::Save(OptionsConfigPtr options)
{
	options->SetHighlightCaretLine( m_highlighyCaretLine->IsChecked() );
	options->SetDisplayLineNumbers( m_displayLineNumbers->IsChecked() );
	options->SetShowIndentationGuidelines( m_showIndentationGuideLines->IsChecked() );
	options->SetIndentUsesTabs(m_indentsUsesTabs->IsChecked());
	options->SetCaretLineColour(m_caretLineColourPicker->GetColour());
	options->SetHighlightMatchedBraces(m_checkBoxMatchBraces->IsChecked());

	// save the tab width
	int value = m_spinCtrlTabWidth->GetValue();

	// make sure we are saving correct values
	if (value < 1 || value > 8) {
		value = 4;
	}
	// save it to configuration file
	EditorConfigST::Get()->SaveLongValue(wxT("EditorTabWidth"), value);

	// save the whitespace visibility
	int style(0); // inivisble
	if (m_whitespaceStyle->GetStringSelection() == wxT("Visible always")) {
		style = 1;
	} else if (m_whitespaceStyle->GetStringSelection() == wxT("Visible after indent")) {
		style = 2;
	}

	options->SetShowWhitspaces(style);
	EditorConfigST::Get()->SaveLongValue(wxT("QuickCodeNavigationUsesMouseMiddleButton"), m_radioBoxNavigationMethod->GetSelection());
	EditorConfigST::Get()->SaveLongValue(wxT("EditorTrimEmptyLines"), m_checkBoxTrimLine->IsChecked() ? 1 : 0);
	EditorConfigST::Get()->SaveLongValue(wxT("EditorAppendLf"), m_checkBoxAppendLF->IsChecked() ? 1 : 0);

    options->SetEdgeMode(m_radioBtnRMLine->GetValue()       ? wxSCI_EDGE_LINE :
                         m_radioBtnRMBackground->GetValue() ? wxSCI_EDGE_BACKGROUND
                                                            : wxSCI_EDGE_NONE);
    options->SetEdgeColumn(m_rightMarginColumn->GetValue());
    options->SetEdgeColour(m_rightMarginColour->GetColour());
}

void EditorSettingsGeneralPage::OnRightMarginIndicator(wxCommandEvent& e)
{
	EnableDisableRightMargin();
}
void EditorSettingsGeneralPage::EnableDisableRightMargin()
{
	if(m_radioBtnRMDisabled->GetValue()){
		m_rightMarginColour->Disable();
		m_rightMarginColumn->Disable();
		m_staticText41->Disable();
		m_staticText5->Disable();
	} else {
		m_rightMarginColour->Enable();
		m_rightMarginColumn->Enable();
		m_staticText41->Enable();
		m_staticText5->Enable();
	}
}
