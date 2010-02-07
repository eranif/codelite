#include "tags_parser_search_path_dlg.h"
#include "windowattrmanager.h"

TagsParserSearchPathsDlg::TagsParserSearchPathsDlg( wxWindow* parent, const wxArrayString &paths, const wxArrayString &excludePaths )
		: TagsParserSearchPathsBaseDlg( parent )
{
	m_checkListPaths->Append( paths );
	for(unsigned int i=0; i<m_checkListPaths->GetCount(); i++) {
		m_checkListPaths->Check(i, true);
	}
	
	m_checkListExcludePaths->Append( excludePaths );
	for(unsigned int i=0; i<m_checkListExcludePaths->GetCount(); i++) {
		m_checkListExcludePaths->Check(i, true);
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

wxArrayString TagsParserSearchPathsDlg::GetExcludePath() const
{
	wxArrayString paths;
	for(unsigned int i=0; i<m_checkListExcludePaths->GetCount(); i++) {
		if(m_checkListExcludePaths->IsChecked(i)) {
			paths.Add( m_checkListExcludePaths->GetString(i) );
		}
	}
	return paths;
}
