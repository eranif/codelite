#include "code_completion_page.h"
#include "localworkspace.h"
#include <wx/tokenzr.h>
#include "globals.h"

CodeCompletionPage::CodeCompletionPage(wxWindow *parent, int type)
	: CodeCompletionBasePage(parent)
	, m_type(type)
{
	if(m_type == TypeWorkspace) {
		
		wxArrayString excludePaths, includePaths;
		wxString opts;
		LocalWorkspaceST::Get()->GetParserPaths(includePaths, excludePaths);
		LocalWorkspaceST::Get()->GetParserOptions(opts);
		
		m_textCtrlSearchPaths->SetValue( wxImplode(includePaths, wxT("\n")) );
		m_textCtrlCmpOptions->SetValue(opts);
	}
}

CodeCompletionPage::~CodeCompletionPage()
{
}

wxArrayString CodeCompletionPage::GetIncludePaths() const
{
	return wxStringTokenize(m_textCtrlSearchPaths->GetValue(), wxT("\n\r"), wxTOKEN_STRTOK);
}
