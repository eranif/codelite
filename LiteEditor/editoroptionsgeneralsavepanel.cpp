#include "editoroptionsgeneralsavepanel.h"

EditorOptionsGeneralSavePanel::EditorOptionsGeneralSavePanel( wxWindow* parent )
: EditorOptionsGeneralSavePanelBase( parent )
, TreeBookNode<EditorOptionsGeneralSavePanel>()
{
	long trim(0);
	EditorConfigST::Get()->GetLongValue(wxT("EditorTrimEmptyLines"), trim);
	m_checkBoxTrimLine->SetValue(trim ? true : false);

	long appendLf(0);
	EditorConfigST::Get()->GetLongValue(wxT("EditorAppendLf"), appendLf);
	m_checkBoxAppendLF->SetValue(appendLf ? true : false);
}

void EditorOptionsGeneralSavePanel::Save(OptionsConfigPtr options)
{
	EditorConfigST::Get()->SaveLongValue(wxT("EditorTrimEmptyLines"), m_checkBoxTrimLine->IsChecked() ? 1 : 0);
	EditorConfigST::Get()->SaveLongValue(wxT("EditorAppendLf"), m_checkBoxAppendLF->IsChecked() ? 1 : 0);
}
