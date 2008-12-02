#include "editoroptionsgeneralindentationpanel.h"

EditorOptionsGeneralIndentationPanel::EditorOptionsGeneralIndentationPanel( wxWindow* parent )
: EditorOptionsGeneralIndetationPanelBase( parent )
, TreeBookNode<EditorOptionsGeneralIndentationPanel>()
{
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	m_indentsUsesTabs->SetValue(options->GetIndentUsesTabs());
    m_indentWidth->SetValue(options->GetIndentWidth());
    m_tabWidth->SetValue(options->GetTabWidth());
}

void EditorOptionsGeneralIndentationPanel::Save(OptionsConfigPtr options)
{
	options->SetIndentUsesTabs(m_indentsUsesTabs->IsChecked());
    options->SetIndentWidth(m_indentWidth->GetValue());
    options->SetTabWidth(m_tabWidth->GetValue());
}

