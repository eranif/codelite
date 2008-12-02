#include "editorsettingscomments.h"
#include "commentconfigdata.h"

EditorSettingsComments::EditorSettingsComments( wxWindow* parent )
: EditorSettingsCommentsBase( parent )
, TreeBookNode<EditorSettingsComments>()
{
	CommentConfigData data;
	EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

	m_checkBoxContCComment->SetValue( data.GetAddStarOnCComment() );
	m_checkBoxContinueCppComment->SetValue( data.GetContinueCppComment() );
}


void EditorSettingsComments::Save(OptionsConfigPtr options)
{
	CommentConfigData data;
	EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

	data.SetAddStarOnCComment(m_checkBoxContCComment->IsChecked());
	data.SetContinueCppComment(m_checkBoxContinueCppComment->IsChecked());

	EditorConfigST::Get()->WriteObject(wxT("CommentConfigData"), &data);
}
