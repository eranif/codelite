#include "cc_include_files.h"
#include <wx/dirdlg.h>
#include <wx/textdlg.h>
#include <wx/tokenzr.h>
#include "globals.h"

CCIncludeFilesPage::CCIncludeFilesPage( wxWindow* parent, const TagsOptionsData & data )
	: CCIncludeFilesBasePage( parent )
{
	m_textCtrlCtagsSearchPaths->SetValue(wxImplode(data.GetParserSearchPaths(),   wxT("\n")));
	m_textCtrlCtagsExcludePaths->SetValue(wxImplode(data.GetParserExcludePaths(), wxT("\n")));
}

void CCIncludeFilesPage::OnAddSearchPath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString new_path = wxDirSelector(_("Add ctags Parser Search Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if(new_path.IsEmpty() == false) {
		wxString currPathsStr = m_textCtrlCtagsSearchPaths->GetValue();
		wxArrayString currPaths = wxStringTokenize(currPathsStr, wxT("\n\r"), wxTOKEN_STRTOK);
		if(currPaths.Index(new_path) == wxNOT_FOUND) {
			
			currPathsStr.Trim().Trim(false);
			if(currPathsStr.IsEmpty() == false) {
				currPathsStr << wxT("\n");
			}
			currPathsStr << new_path;
			
			m_textCtrlCtagsSearchPaths->SetValue(currPathsStr);
		}
	}
}

void CCIncludeFilesPage::OnAddExcludePath( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString new_path = wxDirSelector(_("Add ctags Parser Exclude Path:"), wxT(""), wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if(new_path.IsEmpty() == false) {
		wxString currPathsStr = m_textCtrlCtagsExcludePaths->GetValue();
		wxArrayString currPaths = wxStringTokenize(currPathsStr, wxT("\n\r"), wxTOKEN_STRTOK);
		if(currPaths.Index(new_path) == wxNOT_FOUND) {
			currPathsStr.Trim().Trim(false);
			if(currPathsStr.IsEmpty() == false) {
				currPathsStr << wxT("\n");
			}
			currPathsStr << new_path;
			m_textCtrlCtagsExcludePaths->SetValue(currPathsStr);
		}
	}
}

void CCIncludeFilesPage::Save(TagsOptionsData& data)
{
	
	data.SetParserSearchPaths ( wxStringTokenize(m_textCtrlCtagsSearchPaths->GetValue(), wxT("\r\n"), wxTOKEN_STRTOK) );
	data.SetParserExcludePaths( wxStringTokenize(m_textCtrlCtagsExcludePaths->GetValue(), wxT("\r\n"), wxTOKEN_STRTOK) );
}

wxArrayString CCIncludeFilesPage::GetIncludePaths() const
{
	return wxStringTokenize(m_textCtrlCtagsSearchPaths->GetValue(), wxT("\r\n"), wxTOKEN_STRTOK);
}
