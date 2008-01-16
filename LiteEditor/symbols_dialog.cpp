#include "symbols_dialog.h"
#include "manager.h"

SymbolsDialog::SymbolsDialog( wxWindow* parent )
: SymbolsDialogBase( parent )
, m_line(wxNOT_FOUND)
, m_file(wxEmptyString)
{
	// Initialise the list control
	m_results->InsertColumn(0, wxT("Symbol"));
	m_results->InsertColumn(1, wxT("Kind"));
	m_results->InsertColumn(2, wxT("File"));
	m_results->InsertColumn(3, wxT("Line"));
}
 
void SymbolsDialog::AddSymbol(const TagEntryPtr &tag, bool sel)
{
	wxListItem info;
	wxString line;
	line << tag->GetLine();

	//-------------------------------------------------------
	// Populate the columns
	//-------------------------------------------------------

	// Set the item display name
	wxString tmp(tag->GetFullDisplayName()), name;
	if(tmp.EndsWith(wxT(": [prototype]"), &name)){
        info.SetText(name);
	}else{
		info.SetText(tmp);
	}

	info.SetColumn(0);
	if(sel == true){
		info.SetState(wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
		m_file = tag->GetFile();
		m_line = tag->GetLine();
		m_project = ManagerST::Get()->GetProjectNameByFile(m_file);
	}
	
	long item = m_results->InsertItem(info);

	// Set the item kind
	info.SetColumn(1);
	info.SetId(item);
	info.SetText(tag->GetKind());
	info.SetState(0);
	m_results->SetItem(info); 
	
	// Set the file name
	info.SetColumn(2);
	info.SetId(item);
	info.SetText(tag->GetFile());
	info.SetState(0);
	m_results->SetItem(info); 

	// set the line number
	info.SetColumn(3);
	info.SetId(item);
	info.SetText(line);
	info.SetState(0);
	m_results->SetItem(info); 
}

void SymbolsDialog::AddSymbols(const std::vector<TagEntryPtr> &tags, size_t sel)
{
	for(size_t i=0; i<tags.size(); i++){ AddSymbol(tags[i], sel == i); }
	m_results->SetColumnWidth(0, wxLIST_AUTOSIZE);
	m_results->SetColumnWidth(1, wxLIST_AUTOSIZE);
	m_results->SetColumnWidth(2, wxLIST_AUTOSIZE);
	m_results->SetColumnWidth(3, wxLIST_AUTOSIZE);
}

void SymbolsDialog::UpdateFileAndLine(wxListEvent &event)
{
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 2;
	info.m_mask = wxLIST_MASK_TEXT;

	if( m_results->GetItem(info) ){
		if(info.m_text.IsEmpty())
			return;
		m_file = info.m_text;
	}

	info.m_col = 3;
	if( m_results->GetItem(info) && !info.m_text.IsEmpty()){
		info.m_text.ToLong( &m_line );
	}

	m_project = ManagerST::Get()->GetProjectNameByFile(m_file);
}

void SymbolsDialog::OnItemSelected(wxListEvent &event)
{
	UpdateFileAndLine(event);
}

void SymbolsDialog::OnItemActivated(wxListEvent &event)
{
	UpdateFileAndLine(event);
	EndModal(wxID_OK);
}


void SymbolsDialog::OnButtonCancel(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

void SymbolsDialog::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(wxID_OK);
}
