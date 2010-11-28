///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "simpletablebase.h"
#include "manager.h"
#include "debugger.h"

///////////////////////////////////////////////////////////////////////////

DebuggerTreeListCtrlBase::DebuggerTreeListCtrlBase( wxWindow* parent,
								  wxWindowID id,
								  bool withButtonsPane,
								  const wxPoint& pos,
								  const wxSize& size,
								  long style )
	: wxPanel( parent, id, pos, size, style )
	, m_withButtons(withButtonsPane)
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	long treeStyle = wxTR_HIDE_ROOT|wxTR_COLUMN_LINES|wxTR_ROW_LINES|wxTR_FULL_ROW_HIGHLIGHT|wxTR_EDIT_LABELS|wxTR_HAS_BUTTONS;
	m_listTable = new wxTreeListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, treeStyle);
	bSizer3->Add( m_listTable, 1, wxEXPAND|wxALL, 1 );

	if(m_withButtons)
	{
		wxBoxSizer* bSizer4;
		bSizer4 = new wxBoxSizer( wxVERTICAL );
		m_button1 = new wxButton( this, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer4->Add( m_button1, 0, wxALL|wxEXPAND, 5 );
		m_button2 = new wxButton( this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer4->Add( m_button2, 0, wxALL|wxEXPAND, 5 );
		m_button3 = new wxButton( this, wxID_ANY, _("Delete All"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer4->Add( m_button3, 0, wxALL|wxEXPAND, 5 );
		bSizer3->Add( bSizer4, 0, wxEXPAND, 5 );
	}
	else
	{
		wxBoxSizer* bSizer4;
		bSizer4 = new wxBoxSizer( wxVERTICAL );
		m_button1 = new wxButton( this, wxID_REFRESH, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer4->Add( m_button1, 0, wxALL|wxEXPAND, 5 );
		bSizer3->Add( bSizer4, 0, wxEXPAND, 5 );
	}

	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

	// Connect Events
	m_listTable->Connect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( DebuggerTreeListCtrlBase::OnListEditLabelBegin ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_TREE_END_LABEL_EDIT,   wxTreeEventHandler( DebuggerTreeListCtrlBase::OnListEditLabelEnd ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_TREE_ITEM_MENU,        wxTreeEventHandler( DebuggerTreeListCtrlBase::OnItemRightClick ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_TREE_KEY_DOWN,         wxTreeEventHandler( DebuggerTreeListCtrlBase::OnListKeyDown ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_TREE_ITEM_EXPANDING,   wxTreeEventHandler( DebuggerTreeListCtrlBase::OnItemExpanding ), NULL, this );

	if(m_withButtons) {
		m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerTreeListCtrlBase::OnNewWatch ), NULL, this );
		m_button1->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerTreeListCtrlBase::OnNewWatchUI ), NULL, this );
		m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerTreeListCtrlBase::OnDeleteWatch ), NULL, this );
		m_button2->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerTreeListCtrlBase::OnDeleteWatchUI ), NULL, this );
		m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerTreeListCtrlBase::OnDeleteAll ), NULL, this );
		m_button3->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerTreeListCtrlBase::OnDeleteAllUI ), NULL, this );
	} else {
		m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerTreeListCtrlBase::OnRefresh ), NULL, this );
		m_button1->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerTreeListCtrlBase::OnRefreshUI ), NULL, this );
	}
}

DebuggerTreeListCtrlBase::~DebuggerTreeListCtrlBase()
{
	// Disconnect Events
	m_listTable->Disconnect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( DebuggerTreeListCtrlBase::OnListEditLabelBegin ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_TREE_END_LABEL_EDIT,   wxTreeEventHandler( DebuggerTreeListCtrlBase::OnListEditLabelEnd ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU,        wxTreeEventHandler( DebuggerTreeListCtrlBase::OnItemRightClick ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_TREE_KEY_DOWN,         wxTreeEventHandler( DebuggerTreeListCtrlBase::OnListKeyDown ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_TREE_ITEM_EXPANDING,   wxTreeEventHandler( DebuggerTreeListCtrlBase::OnItemExpanding ), NULL, this );

	if(m_withButtons) {
		m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerTreeListCtrlBase::OnNewWatch ), NULL, this );
		m_button1->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerTreeListCtrlBase::OnNewWatchUI ), NULL, this );
		m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerTreeListCtrlBase::OnDeleteWatch ), NULL, this );
		m_button2->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerTreeListCtrlBase::OnDeleteWatchUI ), NULL, this );
		m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerTreeListCtrlBase::OnDeleteAll ), NULL, this );
		m_button3->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerTreeListCtrlBase::OnDeleteAllUI ), NULL, this );
	} else {
		m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerTreeListCtrlBase::OnRefresh ), NULL, this );
		m_button1->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DebuggerTreeListCtrlBase::OnRefreshUI ), NULL, this );
	}
}

IDebugger* DebuggerTreeListCtrlBase::DoGetDebugger()
{
	if(!ManagerST::Get()->DbgCanInteract())
		return NULL;

	IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
	return dbgr;
}

void DebuggerTreeListCtrlBase::DoResetItemColour(const wxTreeItemId& item, size_t itemKind)
{
	wxTreeItemIdValue cookieOne;
	wxTreeItemId child = m_listTable->GetFirstChild(item, cookieOne);
	while( child.IsOk() ) {
		DbgTreeItemData *data = (DbgTreeItemData *) m_listTable->GetItemData(child);

		bool resetColor = ((itemKind == 0) || (data && (data->_kind & itemKind)));
		if(resetColor) {
			m_listTable->SetItemTextColour(child, *wxBLACK);
		}

		if(m_listTable->HasChildren(child)) {
			DoResetItemColour(child, itemKind);
		}
		child = m_listTable->GetNextChild(item, cookieOne);
	}
}

void DebuggerTreeListCtrlBase::OnEvaluateVariableObj(const DebuggerEvent& event)
{
	wxString gdbId = event.m_expression;
	wxString value = event.m_evaluated;

	std::map<wxString, wxTreeItemId>::iterator iter = m_gdbIdToTreeId.find(gdbId);
	if( iter != m_gdbIdToTreeId.end() ) {

		wxColour defColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT);
		wxColour redColour = *wxRED;
		wxColour itemColor;

		wxString newValue = value;
		wxString curValue = m_listTable->GetItemText(iter->second, 1);

		if(newValue == curValue || curValue.IsEmpty())
			itemColor = defColour;
		else
			itemColor = redColour;

		m_listTable->SetItemText(iter->second, 1, value);
		m_listTable->SetItemTextColour(iter->second, itemColor);

		// keep the red items IDs in the array
		m_gdbIdToTreeId.erase(iter);
	}
}

void DebuggerTreeListCtrlBase::DoRefreshItemRecursively(IDebugger *dbgr, const wxTreeItemId &item, wxArrayString &itemsToRefresh)
{
	if(itemsToRefresh.IsEmpty())
		return;

	wxTreeItemIdValue cookieOne;
	wxTreeItemId exprItem = m_listTable->GetFirstChild(item, cookieOne);
	while( exprItem.IsOk() ) {

		DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(exprItem));
		if(data) {
			int where = itemsToRefresh.Index(data->_gdbId);
			if(where != wxNOT_FOUND) {
				dbgr->EvaluateVariableObject(data->_gdbId, m_DBG_USERR);
				m_gdbIdToTreeId[data->_gdbId] = exprItem;
				itemsToRefresh.RemoveAt((size_t)where);
			}
		}

		if(m_listTable->HasChildren(exprItem)) {
			DoRefreshItemRecursively(dbgr, exprItem, itemsToRefresh);
		}
		exprItem = m_listTable->GetNextChild(item, cookieOne);
	}
}

void DebuggerTreeListCtrlBase::Clear()
{
	wxTreeItemId root = m_listTable->GetRootItem();
	if(root.IsOk()) {
		if(m_listTable->HasChildren(root)) {
			wxTreeItemIdValue cookie;
			wxTreeItemId item = m_listTable->GetFirstChild(root, cookie);
			
			while(item.IsOk()) {
				DoDeleteWatch(item);
				item = m_listTable->GetNextChild(root, cookie);
			}
			
			m_listTable->DeleteChildren(root);
		}
	}

	m_listChildItemId.clear();
	m_createVarItemId.clear();
	m_gdbIdToTreeId.clear();
	m_curStackInfo.Clear();
}

void DebuggerTreeListCtrlBase::DoRefreshItem(IDebugger* dbgr, const wxTreeItemId& item, bool forceCreate)
{
	if(!dbgr || !item.IsOk())
		return;

	DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(item));
	if(data && data->_gdbId.IsEmpty() == false) {
		dbgr->EvaluateVariableObject(data->_gdbId, m_DBG_USERR);
		m_gdbIdToTreeId[data->_gdbId] = item;

	} else if(data && forceCreate) {

		// try to re-create this variable object
		if(m_withButtons) { 
			// HACK: m_withButton is set to true when we are in the context of 
			// the 'Watches' table
			dbgr->CreateVariableObject(m_listTable->GetItemText(item), true, m_DBG_USERR);
		} else {
			dbgr->CreateVariableObject(m_listTable->GetItemText(item), false, m_DBG_USERR);
		}
		
		m_createVarItemId[m_listTable->GetItemText(item)] = item;
	}
}

wxString DebuggerTreeListCtrlBase::DoGetGdbId(const wxTreeItemId& item)
{
	wxString gdbId;
	if(!item.IsOk())
		return gdbId;

	DbgTreeItemData *data = (DbgTreeItemData*) m_listTable->GetItemData(item);
	if(data) {
		return data->_gdbId;
	}
	return gdbId;
}

wxTreeItemId DebuggerTreeListCtrlBase::DoFindItemByGdbId(const wxString& gdbId)
{
	wxTreeItemId root = m_listTable->GetRootItem();
	wxTreeItemIdValue cookieOne;
	wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
	while( item.IsOk() ) {

		wxString id = DoGetGdbId(item);
		if(id.IsEmpty() == false && id == gdbId)
			return item;

		item = m_listTable->GetNextChild(root, cookieOne);
	}
	return wxTreeItemId();
}

void DebuggerTreeListCtrlBase::DoDeleteWatch(const wxTreeItemId& item)
{
	IDebugger *dbgr = DoGetDebugger();
	if(!dbgr || !item.IsOk()) {
		return;
	}

	wxString gdbId = DoGetGdbId(item);
	if(gdbId.IsEmpty() == false) {
		dbgr->DeleteVariableObject(gdbId);
	}

#ifdef __WXMAC__

	// Mac's GDB does not delete all the children of the variable object
	// instead we will do it manually

	if(m_listTable->HasChildren(item)) {
		// Delete this item children
		wxTreeItemIdValue cookie;
		wxTreeItemId child = m_listTable->GetFirstChild(item, cookie);
		while(child.IsOk()) {
			gdbId = DoGetGdbId(child);
			if(gdbId.IsEmpty() == false) {
				dbgr->DeleteVariableObject(gdbId);
			}

			if(m_listTable->HasChildren(child)) {
				DoDeleteWatch(child);
			}

			child = m_listTable->GetNextChild(item, cookie);
		}
	}
#endif

}

wxTreeItemId DebuggerTreeListCtrlBase::DoFindItemByExpression(const wxString& expr)
{
	wxTreeItemId root = m_listTable->GetRootItem();
	wxTreeItemIdValue cookieOne;
	wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
	while( item.IsOk() ) {

		if(m_listTable->GetItemText(item) == expr)
			return item;
		item = m_listTable->GetNextChild(root, cookieOne);
	}
	return wxTreeItemId();
}

void DebuggerTreeListCtrlBase::ResetTableColors()
{
	DoResetItemColour(m_listTable->GetRootItem(), 0);
}

wxString DebuggerTreeListCtrlBase::GetItemPath(const wxTreeItemId &item)
{
	wxArrayString pathArr;
	if(item.IsOk() == false)
		return wxT("");

	DbgTreeItemData* data = (DbgTreeItemData*) m_listTable->GetItemData(item);
	if(data && data->_gdbId.IsEmpty()) {
		// not a variable object item
		return m_listTable->GetItemText(item);
	}

	wxTreeItemId parent = item;
	while(parent.IsOk() && m_listTable->GetRootItem() != parent) {
		DbgTreeItemData* itemData = (DbgTreeItemData*) m_listTable->GetItemData(parent);
		if(itemData && !itemData->_isFake) {
			pathArr.Add(m_listTable->GetItemText(parent));
		}
		parent = m_listTable->GetItemParent(parent);
	}

	if(pathArr.IsEmpty())
		return wxT("");

	wxString itemPath;
	for(int i=(int)pathArr.GetCount()-1; i>=0; i--) {
		itemPath << pathArr.Item(i) << wxT(".");
	}
	itemPath.RemoveLast();
	return itemPath;
}

void DebuggerTreeListCtrlBase::OnCreateVariableObjError(const DebuggerEvent& event)
{
	// failed to create a variable object!
	// remove this expression from the table
	wxTreeItemId root = m_listTable->GetRootItem();
	wxTreeItemIdValue cookieOne;
	wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
	while( item.IsOk() ) {

		if(event.m_expression == m_listTable->GetItemText(item)) {
			m_listTable->Delete(item);
			break;
		}
		item = m_listTable->GetNextChild(root, cookieOne);
	}
}
