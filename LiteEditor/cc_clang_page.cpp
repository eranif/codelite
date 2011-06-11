#include "cc_clang_page.h"
#include "clang_code_completion.h"

CCClangPage::CCClangPage(wxWindow* parent, const TagsOptionsData &data)
	: CCClangBasePage(parent)
{
	m_checkBoxEnableClangCC->SetValue(data.GetClangOptions() & CC_CLANG_ENABLED);
	m_filePickerClang->SetPath(data.GetClangBinary());
}

CCClangPage::~CCClangPage()
{
}

void CCClangPage::Save(TagsOptionsData& data)
{
	size_t options (0);
	if(m_checkBoxEnableClangCC->IsChecked())
		options |= CC_CLANG_ENABLED;

	data.SetClangOptions(options);
	data.SetClangBinary(m_filePickerClang->GetPath());
}

void CCClangPage::OnClangCCEnabledUI(wxUpdateUIEvent& event)
{
	event.Enable(m_checkBoxEnableClangCC->IsChecked());
}

void CCClangPage::OnClearClangCache(wxCommandEvent& event)
{
	ClangCodeCompletion::Instance()->GetCache().Clear();
}

void CCClangPage::OnClearClangCacheUI(wxUpdateUIEvent& event)
{
	event.Enable(m_checkBoxEnableClangCC->IsChecked() && !ClangCodeCompletion::Instance()->GetCache().IsEmpty());
}
