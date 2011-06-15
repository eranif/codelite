#ifndef CCCLANGPAGE_H
#define CCCLANGPAGE_H

#include "tags_options_base_dlg.h" // Base class: CCClangBasePage
#include "tags_options_data.h"
#include "tags_options_dlg.h"

class CCClangPage : public CCClangBasePage , public TagsOptionsPageBase
{
public:
	CCClangPage(wxWindow* parent, const TagsOptionsData &data);
	virtual ~CCClangPage();

	virtual void OnClangCCEnabledUI(wxUpdateUIEvent& event);
	virtual void Save(TagsOptionsData& data);
	virtual void OnClearClangCache(wxCommandEvent& event);
	virtual void OnClearClangCacheUI(wxUpdateUIEvent& event);
	virtual void OnSuggestSearchPaths(wxCommandEvent& event);

};

#endif // CCCLANGPAGE_H
