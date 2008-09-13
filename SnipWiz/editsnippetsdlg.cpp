#include "editsnippetsdlg.h"
#include "snipwiz.h"
#include <wx/msgdlg.h>
#include "AboutHtml.h"

EditSnippetsDlg::EditSnippetsDlg( wxWindow* parent, SnipWiz *plugin )
		: EditSnippetsBaseDlg( parent )
		, m_pPlugin(plugin)
{
	Initialize();
	m_listBox1->SetFocus();
}

void EditSnippetsDlg::OnItemSelected( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString selection = m_listBox1->GetStringSelection();
	m_textCtrlMenuEntry->SetValue(selection);
	m_textCtrlSnippet->SetValue(GetStringDb()->GetSnippetString(selection));
}

void EditSnippetsDlg::OnAddSnippet( wxCommandEvent& event )
{
	if (GetStringDb()->IsSnippetKey(m_textCtrlMenuEntry->GetValue())) {
		wxMessageBox(wxT("Menu entry is not unique!"));
		return;
	}
	GetStringDb()->SetSnippetString(m_textCtrlMenuEntry->GetValue(), m_textCtrlSnippet->GetValue());
	long index = m_listBox1->Append(m_textCtrlMenuEntry->GetValue());
	m_listBox1->SetSelection(index);
	m_modified = true;
}

void EditSnippetsDlg::OnAddSnippetUI( wxUpdateUIEvent& event )
{
	if (m_textCtrlMenuEntry->GetValue().IsEmpty() || m_textCtrlSnippet->GetValue().IsEmpty())
		event.Enable(false);
	else
		event.Enable(true);
}

void EditSnippetsDlg::OnChangeSnippet( wxCommandEvent& event )
{
	wxString curListKey = m_listBox1->GetStringSelection();
	int index = m_listBox1->GetSelection();
	
	// check if list key is not equal new menu entry and if it is an used key
	if (curListKey.Cmp(m_textCtrlMenuEntry->GetValue()) != 0 && GetStringDb()->IsSnippetKey(m_textCtrlMenuEntry->GetValue())) {
		::wxMessageBox(wxT("Menu entry is not unique!"));
		return;
	}
	
	// if menu entry has changed, delete old entry in list
	if(curListKey.Cmp(m_textCtrlMenuEntry->GetValue()) != 0)
		GetStringDb()->DeleteSnippetKey(curListKey);
		
	GetStringDb()->SetSnippetString(m_textCtrlMenuEntry->GetValue(), m_textCtrlSnippet->GetValue());
	m_listBox1->SetString(index, m_textCtrlMenuEntry->GetValue());
	m_modified = true;
}

void EditSnippetsDlg::OnChangeSnippetUI( wxUpdateUIEvent& event )
{
	if (m_textCtrlMenuEntry->GetValue().IsEmpty() || m_textCtrlSnippet->GetValue().IsEmpty())
		event.Enable(false);
	else
		event.Enable(true);
}

void EditSnippetsDlg::OnRemoveSnippet( wxCommandEvent& event )
{
	wxString key = m_listBox1->GetStringSelection();
	int index = m_listBox1->GetSelection();
	
	GetStringDb()->DeleteSnippetKey(key);
	m_listBox1->Delete(index);

	if (m_listBox1->GetCount())
		SelectItem(0);
	m_modified = true;
}

void EditSnippetsDlg::OnRemoveSnippetUI( wxUpdateUIEvent& event )
{
	if (m_listBox1->GetSelection() != wxNOT_FOUND)
		event.Enable(true);
	else
		event.Enable(false);
}

void EditSnippetsDlg::Initialize()
{
	wxTextAttr attribs = m_textCtrlSnippet->GetDefaultStyle();
	wxArrayInt tabs = attribs.GetTabs();
	int tab = 70;
	for (int i = 1; i < 20; i++){
		tabs.Add(tab * i);
	}
	
	attribs.SetTabs(tabs);
	m_textCtrlSnippet->SetDefaultStyle(attribs);
	
	wxArrayString keys;
	GetStringDb()->GetAllSnippetKeys(keys);
	m_listBox1->Append(keys);

	if(m_listBox1->IsEmpty() == false) {
		SelectItem(0);
	}
	m_htmlWinAbout->SetPage(wxString::FromUTF8(snipwizhtml_txt));
}

void EditSnippetsDlg::SelectItem(long index)
{
	m_listBox1->SetSelection(index);
	wxString snippetStr = m_listBox1->GetString((unsigned int)index);
	
	m_textCtrlMenuEntry->SetValue(snippetStr);
	m_textCtrlSnippet->SetValue(GetStringDb()->GetSnippetString(snippetStr));
}

swStringDb* EditSnippetsDlg::GetStringDb()
{
	return m_pPlugin->GetStringDb();
}
