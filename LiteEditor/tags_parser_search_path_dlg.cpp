#include "tags_parser_search_path_dlg.h"
#include "windowattrmanager.h"

TagsParserSearchPathsDlg::TagsParserSearchPathsDlg( wxWindow* parent, const wxArrayString &paths )
		: TagsParserSearchPathsBaseDlg( parent )
{
	m_checkListPaths->Append( paths );
	for(unsigned int i=0; i<m_checkListPaths->GetCount(); i++) {
		m_checkListPaths->Check(i, true);
	}
	WindowAttrManager::Load(this, wxT("TagsParserSearchPathsDlg"), NULL);
}

wxArrayString TagsParserSearchPathsDlg::GetSearchPaths() const
{
	wxArrayString paths;
	for(unsigned int i=0; i<m_checkListPaths->GetCount(); i++) {
		if(m_checkListPaths->IsChecked(i)) {
			paths.Add( m_checkListPaths->GetString(i) );
		}
	}
	return paths;
}


TagsParserSearchPathsDlg::~TagsParserSearchPathsDlg()
{
	WindowAttrManager::Save(this, wxT("TagsParserSearchPathsDlg"), NULL);
}
