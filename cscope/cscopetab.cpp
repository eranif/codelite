#include "cscopetab.h"
#include "cscopedbbuilderthread.h"
#include "imanager.h"
#include "workspace.h"

CscopeTab::CscopeTab( wxWindow* parent, IManager *mgr )
		: CscopeTabBase( parent )
		, m_table(NULL)
		, m_mgr(mgr)
{
}

void CscopeTab::OnItemActivated( wxTreeEvent& event )
{
	wxTreeItemId item = event.GetItem();
	DoItemActivated(item, event);
}

void CscopeTab::Clear()
{
	if (m_table) {
		//free the old table
		FreeTable();
	}
	m_treeCtrlResults->DeleteAllItems();
}

void CscopeTab::BuildTable(CscopeResultTable *table)
{
	if ( !table ) {
		return;
	}

	if (m_table) {
		//free the old table
		FreeTable();
	}

	m_table = table;
	m_treeCtrlResults->DeleteAllItems();

	//add hidden root
	wxTreeItemId root = m_treeCtrlResults->AddRoot(wxT("Root"));

	CscopeResultTable::iterator iter = m_table->begin();
	for (; iter != m_table->end(); iter++ ) {
		wxString file = iter->first;

		//add item for this file
		wxTreeItemId parent;

		std::vector< CscopeEntryData >* vec = iter->second;
		std::vector< CscopeEntryData >::iterator it = vec->begin();
		for ( ; it != vec->end(); it++ ) {
			CscopeEntryData entry = *it;
			if (parent.IsOk() == false) {
				//add parent item
				CscopeEntryData parent_entry = entry;
				parent_entry.SetKind(KindFileNode);
				parent = m_treeCtrlResults->AppendItem(root, entry.GetFile(), wxNOT_FOUND, wxNOT_FOUND, new CscopeTabClientData(parent_entry));
			}

			wxString display_string;
			display_string << wxT("Line: ") << entry.GetLine() << wxT(", Scope: ") << entry.GetScope() << wxT(", Pattern: ") << entry.GetPattern();
			m_treeCtrlResults->AppendItem(parent, display_string, wxNOT_FOUND, wxNOT_FOUND, new CscopeTabClientData(entry));
		}
	}
	FreeTable();
}

void CscopeTab::FreeTable()
{
	if (m_table) {
		CscopeResultTable::iterator iter = m_table->begin();
		for (; iter != m_table->end(); iter++ ) {
			//delete the vector
			delete iter->second;
		}
		m_table->clear();
		delete m_table;
		m_table = NULL;
	}
}
void CscopeTab::OnLeftDClick(wxMouseEvent &event)
{
	// Make sure the double click was done on an actual item
	int flags = wxTREE_HITTEST_ONITEMLABEL;
	wxTreeItemId item = m_treeCtrlResults->GetSelection();
	if ( item.IsOk() ) {
		if ( m_treeCtrlResults->HitTest(event.GetPosition() , flags) == item ) {
			DoItemActivated( item, event );
			return;
		}
	}
	event.Skip();
}

void CscopeTab::DoItemActivated( wxTreeItemId &item, wxEvent &event )
{
	if (item.IsOk()) {
		CscopeTabClientData *data = (CscopeTabClientData*) m_treeCtrlResults->GetItemData(item);
		if (data) {
			wxString wsp_path = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);

			if (data->GetEntry().GetKind() == KindSingleEntry) {

				//a single entry was activated, open the file
				//convert the file path to absolut path. We do it here, to improve performance
				wxFileName fn(data->GetEntry().GetFile());

				if ( !fn.MakeAbsolute(wsp_path) ) {
					wxLogMessage(wxT("failed to convert file to absolute path"));
				}

				m_mgr->OpenFile(fn.GetFullPath(), wxEmptyString, data->GetEntry().GetLine()-1);
				return;
			} else if (data->GetEntry().GetKind() == KindFileNode) {
				//open the file but dont place the caret at a give line
				m_mgr->OpenFile(data->GetEntry().GetFile(), wxEmptyString);
				return;
			}
		}
	}
	event.Skip();
}

void CscopeTab::SetMessage(const wxString &msg, int percent)
{
	m_statusMessage->SetLabel(msg);
	m_gauge->SetValue(percent);
}
