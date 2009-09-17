#ifndef __settersgetterstreectrl__
#define __settersgetterstreectrl__

#include <checkdirtreectrl.h>
#include "entry.h"

//----------------------------------------------------
class SettersGettersTreeData : public wxTreeItemData
{
public:
	enum { Kind_Getter = 0, Kind_Setter, Kind_Parent, Kind_Root };

	TagEntryPtr m_tag;
	int         m_kind;
public:
	SettersGettersTreeData(TagEntryPtr tag, int kind) : m_tag(tag), m_kind(kind) {}
	virtual ~SettersGettersTreeData() {}
};

class SettersGettersTreeCtrl : public wxCheckTreeCtrl {

public:
	SettersGettersTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT("treeCtrl"));
	virtual ~SettersGettersTreeCtrl();

public:
	virtual wxTreeItemId AddRoot   (const wxString& text, bool checked, wxTreeItemData* data = NULL);
	virtual wxTreeItemId AppendItem(const wxTreeItemId& parent, const wxString& text, bool checked, wxTreeItemData* data = NULL);

	DECLARE_EVENT_TABLE();
	void OnItemChecked   (wxCheckTreeCtrlEvent &e);
	void OnItemUnchecked (wxCheckTreeCtrlEvent &e);
};
#endif // __settersgetterstreectrl__
