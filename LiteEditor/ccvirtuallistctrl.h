#ifndef __ccvirtuallistctrl__
#define __ccvirtuallistctrl__

#include <wx/listctrl.h>
#include <vector>

struct CCItemInfo {
	int imgId;
	wxString displayName;
	
	void Reset() {
		imgId = wxNOT_FOUND;
		displayName = wxEmptyString;
	}
};

class CCVirtualListCtrl : public wxListCtrl {
	std::vector<CCItemInfo> m_tags;
public:
	CCVirtualListCtrl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 486,300 ), long style = wxTAB_TRAVERSAL );
	~CCVirtualListCtrl();

public:

	void SetItems(const std::vector<CCItemInfo> &tags) { m_tags = tags; }
	
	virtual wxListItemAttr* OnGetItemAttr(long item) const;
	virtual int OnGetItemColumnImage(long item, long column) const;
	virtual int OnGetItemImage(long item) const;
	virtual wxString OnGetItemText(long item, long column) const;
	int FindMatch(const wxString &word);
	
};
#endif // __ccvirtuallistctrl__
