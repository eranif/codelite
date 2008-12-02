#include "editorsettingsbookmarkspanel.h"

EditorSettingsBookmarksPanel::EditorSettingsBookmarksPanel( wxWindow* parent )
: EditorSettingsBookmarksBasePanel( parent )
, TreeBookNode<EditorSettingsBookmarksPanel>()
{
	m_backgroundColor->SetColour(wxColour(255,0,255));
	m_foregroundColor->SetColour(wxColour(255,0,255));
	m_highlightColor->SetColour(wxString(wxT("LIGHT BLUE")));

	//get the editor's options from the disk
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	m_displaySelection->SetValue(options->GetDisplayBookmarkMargin());
	m_bookMarkShape->SetStringSelection(options->GetBookmarkShape());
	m_backgroundColor->SetColour(options->GetBookmarkBgColour());
	m_foregroundColor->SetColour(options->GetBookmarkFgColour());

	wxString val1 = EditorConfigST::Get()->GetStringValue(wxT("WordHighlightColour"));
	if (val1.IsEmpty() == false) {
		m_highlightColor->SetColour(val1);
	}
}


void EditorSettingsBookmarksPanel::Save(OptionsConfigPtr options)
{
	options->SetDisplayBookmarkMargin( m_displaySelection->IsChecked() );
	options->SetBookmarkShape( m_bookMarkShape->GetStringSelection());
	options->SetBookmarkBgColour( m_backgroundColor->GetColour() );
	options->SetBookmarkFgColour( m_foregroundColor->GetColour() );
	EditorConfigST::Get()->SaveStringValue(wxT("WordHighlightColour"), m_highlightColor->GetColour().GetAsString());
}
