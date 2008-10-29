#include <wx/wxscintilla.h>
#include "editor_config.h"
#include "editorsettingsgeneralpage.h"

EditorSettingsGeneralPage::EditorSettingsGeneralPage( wxWindow* parent )
		: EditorSettingsGeneralPageBase( parent )
{
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    
	m_displayLineNumbers->SetValue(options->GetDisplayLineNumbers());
	m_checkBoxMatchBraces->SetValue(options->GetHighlightMatchedBraces());    
	m_showIndentationGuideLines->SetValue(options->GetShowIndentationGuidelines());
    
	m_highlighyCaretLine->SetValue(options->GetHighlightCaretLine());
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

	m_indentsUsesTabs->SetValue(options->GetIndentUsesTabs());
    m_indentWidth->SetValue(options->GetIndentWidth());
    m_tabWidth->SetValue(options->GetTabWidth());

    m_radioBtnRMDisabled->SetValue(options->GetEdgeMode() == wxSCI_EDGE_NONE);
    m_radioBtnRMLine->SetValue(options->GetEdgeMode() == wxSCI_EDGE_LINE);
    m_radioBtnRMBackground->SetValue(options->GetEdgeMode() == wxSCI_EDGE_BACKGROUND);
    m_rightMarginColumn->SetValue(options->GetEdgeColumn());
    m_rightMarginColour->SetColour(options->GetEdgeColour());
	EnableDisableRightMargin();

	long trim(0);
	EditorConfigST::Get()->GetLongValue(wxT("EditorTrimEmptyLines"), trim);
	m_checkBoxTrimLine->SetValue(trim ? true : false);

	long appendLf(0);
	EditorConfigST::Get()->GetLongValue(wxT("EditorAppendLf"), appendLf);
	m_checkBoxAppendLF->SetValue(appendLf ? true : false);

	long value = 0;
	EditorConfigST::Get()->GetLongValue(wxT("QuickCodeNavigationUsesMouseMiddleButton"), value);
	m_radioBoxNavigationMethod->SetSelection(0);
	if(value == 1){
		m_radioBoxNavigationMethod->SetSelection(1);
	}
}

void EditorSettingsGeneralPage::Save(OptionsConfigPtr options)
{
	options->SetDisplayLineNumbers( m_displayLineNumbers->IsChecked() );
	options->SetHighlightMatchedBraces(m_checkBoxMatchBraces->IsChecked());
	options->SetShowIndentationGuidelines( m_showIndentationGuideLines->IsChecked() );
	options->SetHighlightCaretLine( m_highlighyCaretLine->IsChecked() );
    
	options->SetCaretLineColour(m_caretLineColourPicker->GetColour());

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

    options->SetIndentUsesTabs(m_indentsUsesTabs->IsChecked());
    options->SetIndentWidth(m_indentWidth->GetValue());
    options->SetTabWidth(m_tabWidth->GetValue());

    options->SetEdgeMode(m_radioBtnRMLine->GetValue()       ? wxSCI_EDGE_LINE :
                         m_radioBtnRMBackground->GetValue() ? wxSCI_EDGE_BACKGROUND
                                                            : wxSCI_EDGE_NONE);
    options->SetEdgeColumn(m_rightMarginColumn->GetValue());
    options->SetEdgeColour(m_rightMarginColour->GetColour());

	EditorConfigST::Get()->SaveLongValue(wxT("EditorTrimEmptyLines"), m_checkBoxTrimLine->IsChecked() ? 1 : 0);
	EditorConfigST::Get()->SaveLongValue(wxT("EditorAppendLf"), m_checkBoxAppendLF->IsChecked() ? 1 : 0);
    
	EditorConfigST::Get()->SaveLongValue(wxT("QuickCodeNavigationUsesMouseMiddleButton"), m_radioBoxNavigationMethod->GetSelection());
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

void EditorSettingsGeneralPage::OnHighlightCaretLine(wxCommandEvent& e)
{
    EnableDisableCaretLineColourPicker();
}

void EditorSettingsGeneralPage::EnableDisableCaretLineColourPicker()
{
    if (m_highlighyCaretLine->IsChecked()) {
        m_staticText9->Enable();
        m_caretLineColourPicker->Enable();
    } else {
        m_staticText9->Disable();
        m_caretLineColourPicker->Disable();
    }
}

