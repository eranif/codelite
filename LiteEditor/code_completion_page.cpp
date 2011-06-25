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
		wxString macros;
		
		LocalWorkspaceST::Get()->GetParserPaths(includePaths, excludePaths);
		LocalWorkspaceST::Get()->GetParserOptions(opts);
		LocalWorkspaceST::Get()->GetParserMacros(macros);
		
		m_textCtrlSearchPaths->SetValue( wxImplode(includePaths, wxT("\n")) );
		m_textCtrlCmpOptions->SetValue(opts);
		m_textCtrlMacros->SetValue(macros );
		
	}
}

CodeCompletionPage::~CodeCompletionPage()
{
}

wxArrayString CodeCompletionPage::GetIncludePaths() const
{
	return wxStringTokenize(m_textCtrlSearchPaths->GetValue(), wxT("\n\r"), wxTOKEN_STRTOK);
}

wxString CodeCompletionPage::GetCmpOptions() const
{
	return m_textCtrlCmpOptions->GetValue();
}

wxString CodeCompletionPage::GetMacros() const
{
	return m_textCtrlMacros->GetValue();
}

wxString CodeCompletionPage::GetIncludePathsAsString() const
{
	return m_textCtrlSearchPaths->GetValue();
}

void CodeCompletionPage::Save()
{
	if(m_type == TypeWorkspace) {
		LocalWorkspaceST::Get()->SetParserPaths(GetIncludePaths(), wxArrayString());
		LocalWorkspaceST::Get()->SetParserMacros(GetMacros());
		LocalWorkspaceST::Get()->SetParserOptions(GetCmpOptions());
		LocalWorkspaceST::Get()->Flush();
	}
}
