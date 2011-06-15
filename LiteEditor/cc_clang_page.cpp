#include "cc_clang_page.h"
#include <wx/utils.h>
#include <wx/tokenzr.h>
#include "clang_code_completion.h"
#include <wx/stdpaths.h>
#include "editor_config.h"
#include "includepathlocator.h"
#include "pluginmanager.h"

CCClangPage::CCClangPage(wxWindow* parent, const TagsOptionsData &data)
	: CCClangBasePage(parent)
{
	m_checkBoxEnableClangCC->SetValue(data.GetClangOptions() & CC_CLANG_ENABLED);
	m_filePickerClang->SetPath(data.GetClangBinary());
	m_checkBoxClangFirst->SetValue(data.GetClangOptions() & CC_CLANG_FIRST);
	m_textCtrlClangOptions->SetValue(data.GetClangCmpOptions());
	m_textCtrlClangMacros->SetValue(data.GetClangMacros());
	m_textCtrlClangSearchPaths->SetValue(data.GetClangSearchPaths());
	
	long val(-1);
	EditorConfigST::Get()->GetLongValue(wxT("cc_clang_sash_1"), val);
	if(val != -1) {
		m_splitter1->SetSashPosition(val);
	}
	
	val = -1;
	EditorConfigST::Get()->GetLongValue(wxT("cc_clang_sash_2"), val);
	if(val != -1) {
		m_splitter3->SetSashPosition(val);
	}
	Layout();
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
	EditorConfigST::Get()->SaveLongValue(wxT("cc_clang_sash_1"), m_splitter1->GetSashPosition());
	EditorConfigST::Get()->SaveLongValue(wxT("cc_clang_sash_2"), m_splitter3->GetSashPosition());
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
	data.SetClangCmpOptions(m_textCtrlClangOptions->GetValue());
	data.SetClangMacros(m_textCtrlClangMacros->GetValue());
	data.SetClangSearchPaths(m_textCtrlClangSearchPaths->GetValue());
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

void CCClangPage::OnSuggestSearchPaths(wxCommandEvent& event)
{
	wxUnusedVar(event);
	IncludePathLocator locator(PluginManager::Get());
	wxArrayString paths, excludes;
	locator.Locate(paths, excludes, false);
	
	wxString suggestedPaths;
	for(size_t i=0; i<paths.GetCount(); i++) {
		suggestedPaths << paths.Item(i) << wxT("\n");
	}
	
	suggestedPaths.Trim().Trim(false);
	if(m_textCtrlClangSearchPaths->GetValue().Contains(suggestedPaths) == false) {
		wxString newVal = m_textCtrlClangSearchPaths->GetValue();
		newVal.Trim().Trim(false);
		if(newVal.IsEmpty() == false)
			newVal << wxT("\n");
		newVal << suggestedPaths;
		m_textCtrlClangSearchPaths->SetValue(newVal);
	}
}
