#include "new_quick_watch_dlg.h"
#include <wx/timer.h>
#include "windowattrmanager.h"
#include "debuggerobserver.h"

#define TIPTIMERID 34347

class QWTreeData : public wxTreeItemData {
public:
	VariableObjChild _voc;
	QWTreeData(const VariableObjChild &voc) : _voc(voc){}
	virtual ~QWTreeData(){}
};

BEGIN_EVENT_TABLE(DisplayVariableDlg, NewQuickWatch)
EVT_TIMER(TIPTIMERID, DisplayVariableDlg::OnTimer)
END_EVENT_TABLE()
DisplayVariableDlg::DisplayVariableDlg( wxWindow* parent)
		: NewQuickWatch( parent, wxID_ANY, _("Display Variable"), wxDefaultPosition, wxSize(500, 400) )
		, m_leftWindow(false)
{
	Centre();
	WindowAttrManager::Load(this, wxT("NewQuickWatchDlg"), NULL);
	m_timer = new wxTimer(this, TIPTIMERID);
}

DisplayVariableDlg::~DisplayVariableDlg()
{
	m_timer->Stop();
	delete m_timer;
	m_timer = NULL;
	WindowAttrManager::Save(this, wxT("NewQuickWatchDlg"), NULL);
}

void DisplayVariableDlg::OnExpandItem( wxTreeEvent& event )
{
	wxTreeItemId item = event.GetItem();
	if( item.IsOk()) {
		if( m_treeCtrl->ItemHasChildren(item) ) {
			wxTreeItemIdValue kookie;
			wxTreeItemId child = m_treeCtrl->GetFirstChild(item, kookie);
			while ( child.IsOk() ) {
				if( m_treeCtrl->GetItemText(child) == wxT("<dummy>") ) {
					// Dummy node, remove it and ask the debugger for information
					m_treeCtrl->SetItemText(child, wxT("Loading..."));

					QWTreeData *data = (QWTreeData *)m_treeCtrl->GetItemData(item);
					if( data ) {
						// Ask the debugger for information
						m_debugger->ListChildren(data->_voc.gdbId, DBG_USERR_QUICKWACTH);
						m_gdbId2Item[data->_voc.gdbId] = item;
					}
					break;
				}
				child = m_treeCtrl->GetNextChild(item, kookie);
			}
		}
	}
}

void DisplayVariableDlg::BuildTree(const VariableObjChildren& children, IDebugger *debugger)
{
	m_debugger = debugger;
	m_gdbId2Item.clear();
	m_gdbId2ItemLeaf.clear();
	m_treeCtrl->DeleteAllItems();
	wxTreeItemId root = m_treeCtrl->AddRoot( m_variableName );

	if( children.empty() ) return;
	DoAddChildren( root, children );
}

void DisplayVariableDlg::AddItems(const wxString& varname, const VariableObjChildren& children)
{
	std::map<wxString, wxTreeItemId>::iterator iter = m_gdbId2Item.find(varname);
	if( iter != m_gdbId2Item.end() ) {
		wxTreeItemId item = iter->second;
		DoAddChildren( item, children );
	}
}

void DisplayVariableDlg::DoAddChildren(wxTreeItemId& item, const VariableObjChildren& children)
{
	if( item.IsOk() == false ) return;

	if(m_treeCtrl->GetRootItem() != item && m_treeCtrl->ItemHasChildren(item)) {
		// delete the <dummy> node
		wxTreeItemIdValue kookie;
		wxTreeItemId child = m_treeCtrl->GetFirstChild(item, kookie);
		while ( child.IsOk() ) {
			wxString itemText = m_treeCtrl->GetItemText(child);
			if( itemText == wxT("<dummy>") || itemText == wxT("Loading...")) {
				m_treeCtrl->Delete( child );
			}
			child = m_treeCtrl->GetNextChild(item, kookie);
		}
	}

	for(size_t i=0; i<children.size(); i++) {
		VariableObjChild ch = children.at(i);
		if ( ch.varName != wxT("public") && ch.varName != wxT("private") && ch.varName != wxT("protected") ) {
			// Real node
			wxTreeItemId child = m_treeCtrl->AppendItem(item, ch.varName, -1, -1, new QWTreeData(ch));
			if ( ch.numChilds > 0 ) {
				// add fake node to this item, so it will have the [+] on the side
				m_treeCtrl->AppendItem(child, wxT("<dummy>"));
			}
			// ask gdb for the value for this node
			m_debugger->EvaluateVariableObject( ch.gdbId, DBG_USERR_QUICKWACTH );
			m_gdbId2ItemLeaf[ch.gdbId] = child;
		} else {
			// Fake node
			m_debugger->ListChildren(ch.gdbId, DBG_USERR_QUICKWACTH);
			m_gdbId2Item[ch.gdbId] = item;
		}
	}
}

void DisplayVariableDlg::OnBtnCancel(wxCommandEvent& e)
{
	DoCleanUp();
	e.Skip();
}

void DisplayVariableDlg::UpdateValue(const wxString& varname, const wxString& value)
{
	std::map<wxString, wxTreeItemId>::iterator iter = m_gdbId2ItemLeaf.find(varname);
	if( iter != m_gdbId2ItemLeaf.end() ) {
		wxTreeItemId item = iter->second;
		if ( item.IsOk() ) {
			wxString curtext = m_treeCtrl->GetItemText( item );
			curtext << wxT(" = ") << value;
			m_treeCtrl->SetItemText( item, curtext );
		}
	}
}

void DisplayVariableDlg::OnCloseEvent(wxCloseEvent& e)
{
	DoCleanUp();
	e.Skip();
}

void DisplayVariableDlg::DoCleanUp()
{
	if(m_debugger && m_mainVariableObject.IsEmpty() == false) {
		m_debugger->DeleteVariableObject(m_mainVariableObject);
	}
	m_gdbId2Item.clear();
	m_gdbId2ItemLeaf.clear();
	m_mainVariableObject = wxT("");
	m_variableName = wxT("");
}

void DisplayVariableDlg::HideDialog()
{
	DoCleanUp();
	wxDialog::Show(false);
}

void DisplayVariableDlg::OnKeyDown(wxKeyEvent& event)
{
	if( event.GetKeyCode() == WXK_ESCAPE ) {
		HideDialog();
	} else {
		event.Skip();
	}
}

void DisplayVariableDlg::ShowDialog(bool center)
{
	m_treeCtrl->SetFocus();
	if ( center ) {
		Centre();
	} else {
		Move( wxGetMousePosition() );

	}
	wxDialog::Show();
}

void DisplayVariableDlg::OnLeftDown(wxMouseEvent& e)
{
	int flags;
	wxTreeItemId item = m_treeCtrl->HitTest(e.GetPosition(), flags);
	if ( item.IsOk() && m_treeCtrl->ItemHasChildren(item) && (flags & wxTREE_HITTEST_ONITEMLABEL )) {
		if ( m_treeCtrl->IsExpanded(item) ) {
			m_treeCtrl->Collapse( item );
		} else {
			m_treeCtrl->Expand( item );
		}
	}
	e.Skip();
}


void DisplayVariableDlg::OnItemExpanded(wxTreeEvent& event)
{
	event.Skip();
}

void DisplayVariableDlg::OnMouseLeaveWindow(wxMouseEvent& e)
{
	m_leftWindow = true;
	m_timer->Start(250, true);
}

void DisplayVariableDlg::OnMouseEnterWindow(wxMouseEvent& e)
{
	m_leftWindow = false;
	e.Skip();
}

void DisplayVariableDlg::OnTimer(wxTimerEvent& e)
{
	if( m_leftWindow ) {
		m_leftWindow = false;
		HideDialog();
	}
}
