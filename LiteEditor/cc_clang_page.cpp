#include "cc_clang_page.h"
#include <wx/utils.h>
#include <wx/tokenzr.h>
#include "clang_code_completion.h"
#include <wx/stdpaths.h>

CCClangPage::CCClangPage(wxWindow* parent, const TagsOptionsData &data)
	: CCClangBasePage(parent)
{
	m_checkBoxEnableClangCC->SetValue(data.GetClangOptions() & CC_CLANG_ENABLED);
	m_filePickerClang->SetPath(data.GetClangBinary());
	m_checkBoxClangFirst->SetValue(data.GetClangOptions() & CC_CLANG_FIRST);
	m_textCtrlCompilerOptions->SetValue(data.GetClangCmpOptions());
	
#if defined (__WXMSW__)
	if(m_filePickerClang->GetPath().IsEmpty()) {
		wxString defaultClang;
		// try to locate the default binary
		wxFileName exePath(wxStandardPaths::Get().GetExecutablePath());
		defaultClang = exePath.GetPath();
		defaultClang << wxFileName::GetPathSeparator() << wxT("clang++.exe");
		
		if(wxFileName::FileExists(defaultClang)) {
			m_filePickerClang->SetPath(defaultClang);
		}
	}
#endif
}

CCClangPage::~CCClangPage()
{
}

void CCClangPage::Save(TagsOptionsData& data)
{
	size_t options (0);
	if(m_checkBoxEnableClangCC->IsChecked())
		options |= CC_CLANG_ENABLED;
		
	if(m_checkBoxClangFirst->IsChecked())
		options |= CC_CLANG_FIRST;
		
	data.SetClangOptions(options);
	data.SetClangBinary(m_filePickerClang->GetPath());
	data.SetClangCmpOptions(m_textCtrlCompilerOptions->GetValue());
}

void CCClangPage::OnClangCCEnabledUI(wxUpdateUIEvent& event)
{
	event.Enable(m_checkBoxEnableClangCC->IsChecked());
}

void CCClangPage::OnClearClangCache(wxCommandEvent& event)
{
	wxBusyCursor cursor;
	ClangCodeCompletion::Instance()->GetCache().Clear();
}

void CCClangPage::OnClearClangCacheUI(wxUpdateUIEvent& event)
{
	event.Enable(m_checkBoxEnableClangCC->IsChecked() && !ClangCodeCompletion::Instance()->GetCache().IsEmpty());
}
