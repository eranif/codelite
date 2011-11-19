#include "cc_clang_page.h"
#include <wx/utils.h>
#include <wx/tokenzr.h>

#if HAS_LIBCLANG
#    include "clang_code_completion.h"
#endif

#include <wx/filedlg.h>
#include <wx/stdpaths.h>
#include "editor_config.h"
#include "includepathlocator.h"
#include "pluginmanager.h"

CCClangPage::CCClangPage(wxWindow* parent, const TagsOptionsData &data)
	: CCClangBasePage(parent)
{
	
#if HAS_LIBCLANG
	m_checkBoxEnableClangCC->SetValue(data.GetClangOptions() & CC_CLANG_ENABLED);
#else
	m_checkBoxEnableClangCC->SetValue(false);
	m_checkBoxEnableClangCC->Enable(false);
#endif

	m_checkBoxClangFirst->SetValue(data.GetClangOptions() & CC_CLANG_FIRST);
	m_textCtrlClangSearchPaths->SetValue(data.GetClangSearchPaths());
	m_choiceCachePolicy->Clear();
	
	// defaults
	m_choiceCachePolicy->Append(TagsOptionsData::CLANG_CACHE_LAZY);
	m_choiceCachePolicy->Append(TagsOptionsData::CLANG_CACHE_ON_FILE_LOAD);
	m_choiceCachePolicy->Select(1);
	
	int where = m_choiceCachePolicy->FindString(data.GetClangCachePolicy());
	if(where != wxNOT_FOUND) {
		m_choiceCachePolicy->Select(where);
	}
	
	Layout();
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
	data.SetClangSearchPaths(m_textCtrlClangSearchPaths->GetValue());
	data.SetClangCachePolicy(m_choiceCachePolicy->GetStringSelection());
}

void CCClangPage::OnClangCCEnabledUI(wxUpdateUIEvent& event)
{
	event.Enable(m_checkBoxEnableClangCC->IsChecked());
}

void CCClangPage::OnClearClangCache(wxCommandEvent& event)
{
	wxBusyCursor cursor;
#if HAS_LIBCLANG
	ClangCodeCompletion::Instance()->ClearCache();
#endif
}

void CCClangPage::OnClearClangCacheUI(wxUpdateUIEvent& event)
{
#if HAS_LIBCLANG
	event.Enable(m_checkBoxEnableClangCC->IsChecked() && !ClangCodeCompletion::Instance()->IsCacheEmpty());
#else
	event.Enable(false);
#endif
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

void CCClangPage::OnSelectClangPath(wxCommandEvent& event)
{
	wxUnusedVar(event);
}
