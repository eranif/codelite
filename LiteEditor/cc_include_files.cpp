#include "cc_include_files.h"
#include <wx/dirdlg.h>

CCIncludeFilesPage::CCIncludeFilesPage( wxWindow* parent, const TagsOptionsData & data )
	: CCIncludeFilesBasePage( parent )
{
	m_listBoxSearchPaths->Append                   (data.GetParserSearchPaths() );
	m_listBoxSearchPaths1->Append                  (data.GetParserExcludePaths() );
}

void CCIncludeFilesPage::OnAddSearchPath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString new_path = wxDirSelector(_("Add Parser Search Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if(new_path.IsEmpty() == false) {
		if(m_listBoxSearchPaths->GetStrings().Index(new_path) == wxNOT_FOUND) {
			m_listBoxSearchPaths->Append(new_path);
		}
	}
}

void CCIncludeFilesPage::OnAddSearchPathUI( wxUpdateUIEvent& event )
{
	event.Enable(true);
}

void CCIncludeFilesPage::OnRemoveSearchPath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	int sel = m_listBoxSearchPaths->GetSelection();
	if( sel != wxNOT_FOUND) {
		m_listBoxSearchPaths->Delete((unsigned int)sel);
	}
}

void CCIncludeFilesPage::OnRemoveSearchPathUI( wxUpdateUIEvent& event )
{
	event.Enable(m_listBoxSearchPaths->GetSelection() != wxNOT_FOUND);
}

void CCIncludeFilesPage::OnClearAll( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_listBoxSearchPaths->Clear();

}

void CCIncludeFilesPage::OnClearAllUI( wxUpdateUIEvent& event )
{
	event.Enable(m_listBoxSearchPaths->IsEmpty() == false);
}

void CCIncludeFilesPage::OnAddExcludePath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString new_path = wxDirSelector(wxT("Add Parser Search Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if(new_path.IsEmpty() == false) {
		if(m_listBoxSearchPaths1->GetStrings().Index(new_path) == wxNOT_FOUND) {
			m_listBoxSearchPaths1->Append(new_path);
		}
	}
}

void CCIncludeFilesPage::OnAddExcludePathUI( wxUpdateUIEvent& event )
{
	event.Enable(true);
}

void CCIncludeFilesPage::OnRemoveExcludePath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	int sel = m_listBoxSearchPaths1->GetSelection();
	if( sel != wxNOT_FOUND) {
		m_listBoxSearchPaths1->Delete((unsigned int)sel);
	}
}

void CCIncludeFilesPage::OnRemoveExcludePathUI( wxUpdateUIEvent& event )
{
	event.Enable(m_listBoxSearchPaths1->GetSelection() != wxNOT_FOUND);
}

void CCIncludeFilesPage::OnClearAllExcludePaths( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_listBoxSearchPaths1->Clear();
}

void CCIncludeFilesPage::OnClearAllExcludePathsUI( wxUpdateUIEvent& event )
{
	event.Enable(m_listBoxSearchPaths1->IsEmpty() == false);
}

void CCIncludeFilesPage::Save(TagsOptionsData& data)
{
	data.SetParserSearchPaths ( m_listBoxSearchPaths->GetStrings() );
	data.SetParserExcludePaths( m_listBoxSearchPaths1->GetStrings() );
}

wxArrayString CCIncludeFilesPage::GetIncludePaths() const
{
	return m_listBoxSearchPaths->GetStrings();
}
