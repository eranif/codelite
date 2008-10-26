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
}

void EditorSettingsGeneralPage::Save(OptionsConfigPtr options)
{
	options->SetHighlightCaretLine( m_highlighyCaretLine->IsChecked() );
	options->SetDisplayLineNumbers( m_displayLineNumbers->IsChecked() );
	options->SetShowIndentationGuidelines( m_showIndentationGuideLines->IsChecked() );
	options->SetIndentUsesTabs(m_indentsUsesTabs->IsChecked());
	options->SetCaretLineColour(m_caretLineColourPicker->GetColour());

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
}
