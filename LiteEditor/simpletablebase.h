#ifndef __simpletablebase__
#define __simpletablebase__

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include "treelistctrl.h"
#include <map>
#include "debugger.h"
#include "debuggerobserver.h"

///////////////////////////////////////////////////////////////////////////

class DbgTreeItemData : public wxTreeItemData
{
public:
	wxString _gdbId;
	size_t   _kind;

public:
	enum {
		Locals         = 0x00000001,
		FuncArgs       = 0x00000002,
		VariableObject = 0x00000004,
		Watch          = 0x00000010
	};

public:
	DbgTreeItemData()
	: _kind(Locals)
	{}

	DbgTreeItemData(const wxString &gdbId)
	: _gdbId(gdbId)
	{}

	virtual ~DbgTreeItemData()
	{}
};


///////////////////////////////////////////////////////////////////////////////
/// Class SimpleTableBase
///////////////////////////////////////////////////////////////////////////////
class DebuggerTreeListCtrlBase : public wxPanel
{
private:
	bool                             m_withButtons;

protected:
	wxTreeListCtrl*                  m_listTable;
	wxButton*                        m_button1;
	wxButton*                        m_button2;
	wxButton*                        m_button3;
	std::map<wxString, wxTreeItemId> m_gdbIdToTreeId;
	std::map<wxString, wxTreeItemId> m_listChildItemId;
	std::map<wxString, wxTreeItemId> m_createVarItemId;

protected:
	int                              m_DBG_USERR;
	int                              m_QUERY_NUM_CHILDS;
	int                              m_LIST_CHILDS;

protected:
	// Virtual event handlers, overide them in your derived class
	virtual void OnListEditLabelBegin( wxTreeEvent& event ) {
		event.Skip();
	}
	virtual void OnListEditLabelEnd( wxTreeEvent& event ) {
		event.Skip();
	}
	virtual void OnItemRightClick( wxTreeEvent& event ) {
		event.Skip();
	}
	virtual void OnListKeyDown( wxTreeEvent& event ) {
		event.Skip();
	}
	virtual void OnItemExpanding( wxTreeEvent& event ) {
		event.Skip();
	}

	virtual void OnNewWatch( wxCommandEvent& event ) {
		event.Skip();
	}
	virtual void OnNewWatchUI( wxUpdateUIEvent& event ) {
		event.Skip();
	}
	virtual void OnDeleteWatch( wxCommandEvent& event ) {
		event.Skip();
	}
	virtual void OnDeleteWatchUI( wxUpdateUIEvent& event ) {
		event.Skip();
	}
	virtual void OnDeleteAll( wxCommandEvent& event ) {
		event.Skip();
	}
	virtual void OnDeleteAllUI( wxUpdateUIEvent& event ) {
		event.Skip();
	}


public:
	DebuggerTreeListCtrlBase( wxWindow* parent,
	                          wxWindowID id = wxID_ANY,
	                          bool withButtonsPane = true,
	                          const wxPoint& pos = wxDefaultPosition,
	                          const wxSize& size = wxSize( 500,300 ),
	                          long style = wxTAB_TRAVERSAL );
	~DebuggerTreeListCtrlBase();

	//////////////////////////////////////////////
	// Common to both Locals / Watches
	//////////////////////////////////////////////
	virtual IDebugger*   DoGetDebugger           ();
	virtual void         DoResetItemColour       (const wxTreeItemId& item);
	virtual void         OnEvaluateVariableObj   (const DebuggerEvent& event);
	virtual void         DoRefreshItemRecursively(IDebugger *dbgr, const wxTreeItemId &item, wxArrayString &itemsToRefresh);
	virtual void         Clear                   ();
	virtual void         DoRefreshItem           (IDebugger *dbgr, const wxTreeItemId &item, bool forceCreate);
	virtual wxString     DoGetGdbId              (const wxTreeItemId& item);
	virtual wxTreeItemId DoFindItemByGdbId       (const wxString& gdbId);
	virtual void         DoDeleteWatch           (const wxTreeItemId& item);
};

#endif //__simpletablebase__
