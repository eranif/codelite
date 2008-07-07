#ifndef __opentypevlistctrl__
#define __opentypevlistctrl__

#include <wx/listctrl.h>
#include "entry.h"
#include <vector>

class OpenTypeVListCtrl : public wxListView {
	std::vector<TagEntryPtr> m_tags;
	
public:
	OpenTypeVListCtrl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~OpenTypeVListCtrl();

public:
	void SetItems(const std::vector<TagEntryPtr> &tags) { m_tags = tags; }
	
	virtual wxListItemAttr* OnGetItemAttr(long item) const;
	virtual int OnGetItemColumnImage(long item, long column) const;
	virtual int OnGetItemImage(long item) const;
	virtual wxString OnGetItemText(long item, long column) const;
	int FindMatch(const wxString &word);
	TagEntryPtr GetTagAt(long item);
};

#endif // __opentypevlistctrl__

