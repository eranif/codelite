#include "editoroptionsgeneralcodenavpanel.h"

EditorOptionsGeneralCodeNavPanel::EditorOptionsGeneralCodeNavPanel( wxWindow* parent )
: EditorOptionsGeneralCodeNavPanelBase( parent )
, TreeBookNode<EditorOptionsGeneralCodeNavPanel>()
{
	long value = 0;
	EditorConfigST::Get()->GetLongValue(wxT("QuickCodeNavigationUsesMouseMiddleButton"), value);
	switch(value){
	case 1:
		m_radioBoxNavigationMethodLeft->SetValue(false);
		m_radioBoxNavigationMethodMiddle->SetValue(true);
		break;
	case 0:
	default:
		m_radioBoxNavigationMethodLeft->SetValue(true);
		m_radioBoxNavigationMethodMiddle->SetValue(false);
		break;
	}
}


void EditorOptionsGeneralCodeNavPanel::Save(OptionsConfigPtr options)
{
	EditorConfigST::Get()->SaveLongValue(wxT("QuickCodeNavigationUsesMouseMiddleButton"), m_radioBoxNavigationMethodMiddle->GetValue() ? 1 : 0);
}
