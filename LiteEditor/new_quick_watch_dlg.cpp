#include "new_quick_watch_dlg.h"
#include "windowattrmanager.h"
#include "debuggerobserver.h"

class QWTreeData : public wxTreeItemData {
public:
	VariableObjChild _voc;
	QWTreeData(const VariableObjChild &voc) : _voc(voc){}
	virtual ~QWTreeData(){}
};

NewQuickWatchDlg::NewQuickWatchDlg( wxWindow* parent)
		: NewQuickWatch( parent, wxID_ANY, _("Display Variable"), wxDefaultPosition, wxSize(300, 300) )
{
	Centre();

	WindowAttrManager::Load(this, wxT("NewQuickWatchDlg"), NULL);
}

NewQuickWatchDlg::~NewQuickWatchDlg()
{
	WindowAttrManager::Save(this, wxT("NewQuickWatchDlg"), NULL);
}

void NewQuickWatchDlg::OnExpandItem( wxTreeEvent& event )
{
	wxTreeItemId item = event.GetItem();
	if( item.IsOk()) {
		if( m_treeCtrl->ItemHasChildren(item) ) {
			wxTreeItemIdValue kookie;
			wxTreeItemId child = m_treeCtrl->GetFirstChild(item, kookie);
			if( child.IsOk() && m_treeCtrl->GetItemText(child) == wxT("<dummy>") ) {
				// Dummy node, remove it and ask the debugger for information
				m_treeCtrl->SetItemText(child, wxT("Loading..."));

				QWTreeData *data = (QWTreeData *)m_treeCtrl->GetItemData(item);
				if( data ) {
					// Ask the debugger for information
					m_debugger->ListChildren(data->_voc.gdbId, DBG_USERR_QUICKWACTH);
					m_gdbId2Item[data->_voc.gdbId] = item;
				}
			}
		}
	}
}

void NewQuickWatchDlg::BuildTree(const VariableObjChildren& children, IDebugger *debugger)
{
	m_debugger = debugger;
	m_gdbId2Item.clear();
	m_gdbId2ItemLeaf.clear();
	m_treeCtrl->DeleteAllItems();
	wxTreeItemId root = m_treeCtrl->AddRoot( m_variableName );

	if( children.empty() ) return;
	DoAddChildren( root, children );
}

void NewQuickWatchDlg::AddItems(const wxString& varname, const VariableObjChildren& children)
{
	std::map<wxString, wxTreeItemId>::iterator iter = m_gdbId2Item.find(varname);
	if( iter != m_gdbId2Item.end() ) {
		wxTreeItemId item = iter->second;
		DoAddChildren( item, children );
	}
}

void NewQuickWatchDlg::DoAddChildren(wxTreeItemId& item, const VariableObjChildren& children)
{
	if( item.IsOk() == false ) return;

	if(m_treeCtrl->GetRootItem() != item && m_treeCtrl->ItemHasChildren(item)) {
		m_treeCtrl->DeleteChildren(item);
	}

	for(size_t i=0; i<children.size(); i++) {
		VariableObjChild ch = children.at(i);
		wxTreeItemId child = m_treeCtrl->AppendItem(item, ch.varName, -1, -1, new QWTreeData(ch));
		if ( ch.numChilds > 0 ) {
			// add fake node to this item, so it will have the [+] on the side
			m_treeCtrl->AppendItem(child, wxT("<dummy>"));
		}

		// ask gdb for the value for this node
		m_debugger->EvaluateVariableObject( ch.gdbId, DBG_USERR_QUICKWACTH );
		m_gdbId2ItemLeaf[ch.gdbId] = child;
	}
}

void NewQuickWatchDlg::OnBtnCancel(wxCommandEvent& e)
{
	DoCleanUp();
	e.Skip();
}

void NewQuickWatchDlg::UpdateValue(const wxString& varname, const wxString& value)
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

void NewQuickWatchDlg::OnCloseEvent(wxCloseEvent& e)
{
	DoCleanUp();
	e.Skip();
}

void NewQuickWatchDlg::DoCleanUp()
{
	if(m_debugger && m_mainVariableObject.IsEmpty() == false) {
		m_debugger->DeleteVariableObject(m_mainVariableObject);
	}
	m_gdbId2Item.clear();
	m_gdbId2ItemLeaf.clear();
	m_mainVariableObject = wxT("");
	m_variableName = wxT("");
}

void NewQuickWatchDlg::HideDialog()
{
	DoCleanUp();
	wxDialog::Show(false);
}

void NewQuickWatchDlg::OnKeyDown(wxKeyEvent& event)
{
	if( event.GetKeyCode() == WXK_ESCAPE ) {
		HideDialog();
	} else {
		event.Skip();
	}
}

void NewQuickWatchDlg::ShowDialog(bool center)
{
	m_treeCtrl->SetFocus();
	if ( center ) {
		Centre();
	} else {
		Move( wxGetMousePosition() );

	}
	wxDialog::Show();
}
