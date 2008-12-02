#include "editoroptionsgeneralrightmarginpanel.h"
#include <wx/wxscintilla.h>

EditorOptionsGeneralRightMarginPanel::EditorOptionsGeneralRightMarginPanel( wxWindow* parent )
: EditorOptionsGeneralRightMarginPanelBase( parent )
, TreeBookNode<EditorOptionsGeneralRightMarginPanel>()
{
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	m_radioBtnRMDisabled->SetValue(options->GetEdgeMode() == wxSCI_EDGE_NONE);
    m_radioBtnRMLine->SetValue(options->GetEdgeMode() == wxSCI_EDGE_LINE);
    m_radioBtnRMBackground->SetValue(options->GetEdgeMode() == wxSCI_EDGE_BACKGROUND);
    m_rightMarginColumn->SetValue(options->GetEdgeColumn());
    m_rightMarginColour->SetColour(options->GetEdgeColour());
	EnableDisableRightMargin();
}

void EditorOptionsGeneralRightMarginPanel::Save(OptionsConfigPtr options)
{
    options->SetEdgeMode(m_radioBtnRMLine->GetValue()       ? wxSCI_EDGE_LINE :
                         m_radioBtnRMBackground->GetValue() ? wxSCI_EDGE_BACKGROUND
                                                            : wxSCI_EDGE_NONE);
    options->SetEdgeColumn(m_rightMarginColumn->GetValue());
    options->SetEdgeColour(m_rightMarginColour->GetColour());
}

void EditorOptionsGeneralRightMarginPanel::OnRightMarginIndicator(wxCommandEvent& e)
{
	EnableDisableRightMargin();
}

void EditorOptionsGeneralRightMarginPanel::EnableDisableRightMargin()
{
	if(m_radioBtnRMDisabled->GetValue()){
		m_rightMarginColour->Disable();
		m_rightMarginColumn->Disable();
		m_staticText1->Disable();
		m_staticText2->Disable();
	} else {
		m_rightMarginColour->Enable();
		m_rightMarginColumn->Enable();
		m_staticText1->Enable();
		m_staticText2->Enable();
	}
}
