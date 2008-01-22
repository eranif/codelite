#include "cscopetab.h"
#include "cscopedbbuilderthread.h"

CscopeTab::CscopeTab( wxWindow* parent )
		: CscopeTabBase( parent )
		, m_table(NULL)
{
}

void CscopeTab::OnItemActivated( wxTreeEvent& event )
{
}

void CscopeTab::BuildTable(CscopeResultTable *table)
{
	if( !table ) { return; }
		
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
			if(parent.IsOk() == false) {
				//add parent item
				parent = m_treeCtrlResults->AppendItem(root, entry.GetFile(), wxNOT_FOUND, wxNOT_FOUND, new CscopeTabClientData(entry));
			}
			
			wxString display_string;
			display_string << wxT("[") << entry.GetLine() << wxT("] ") << entry.GetPattern();
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
