#include "tasks_find_what_dlg.h"
#include <wx/msgdlg.h>
#include <wx/regex.h>
#include "editor_config.h"
#include "taskspaneldata.h"
#include "windowattrmanager.h"

class TaskData : public wxClientData
{
public:
	wxString m_regex;

public:
	TaskData(const wxString &regex) : m_regex(regex) {}
	virtual ~TaskData() {}
};

TasksFindWhatDlg::TasksFindWhatDlg( wxWindow* parent )
		: TasksFindWhatDlgBase( parent )
{
	m_list->InsertColumn(0, wxT("Enabled"));
	m_list->InsertColumn(1, wxT("Task"));
	m_list->SetColumnWidth(1, 200);

	// Load all info from disk
	TasksPanelData data;
	EditorConfigST::Get()->ReadObject(wxT("TasksPanelData"), &data);

	std::map<wxString, wxString>::const_iterator iter = data.GetTasks().begin();
	for (; iter != data.GetTasks().end(); iter++) {
		DoAddLine(iter->first, iter->second, data.GetEnabledItems().Index(iter->first) != wxNOT_FOUND);
	}
	WindowAttrManager::Load(this, wxT("TasksFindWhatDlg"), NULL);
}

TasksFindWhatDlg::~TasksFindWhatDlg()
{
	WindowAttrManager::Save(this, wxT("TasksFindWhatDlg"), NULL);
}

void TasksFindWhatDlg::OnNewTask( wxCommandEvent& event )
{
	NewTaskDialog dlg(this);
	WindowAttrManager::Load(&dlg, wxT("NewTaskDialog"), NULL);

	dlg.SetLabel(_("New Task"));
	if (dlg.ShowModal() == wxID_OK) {
		wxRegEx re(dlg.m_regex->GetValue());
		if (re.IsValid() == false) {
			wxMessageBox(wxString::Format(wxT("'%s' is not a valid regulare expression"), dlg.m_regex->GetValue().c_str()), wxT("CodeLite"), wxICON_WARNING|wxOK);
			return;
		}
		DoAddLine(dlg.m_name->GetValue(), dlg.m_regex->GetValue(), true);
	}
	WindowAttrManager::Save(&dlg, wxT("NewTaskDialog"), NULL);
}

void TasksFindWhatDlg::OnDeleteTask( wxCommandEvent& event )
{
	int selection = m_list->GetSelection();
	if (selection == wxNOT_FOUND)
		return;

	int answer = wxMessageBox(wxT("Are you sure you want to delete this entry?"), wxT("Confirm"), wxICON_QUESTION|wxYES_NO);
	if (answer == wxYES) {
		m_list->DeleteItem(selection);
	}
}

void TasksFindWhatDlg::OnDeleteTaskUI( wxUpdateUIEvent& event )
{
	event.Enable(m_list->GetItemCount() && m_list->GetSelection() != wxNOT_FOUND);
}

void TasksFindWhatDlg::OnEditTask(wxCommandEvent& event)
{
	int selection = m_list->GetSelection();
	if (selection != wxNOT_FOUND) {

		NewTaskDialog dlg(this);
		WindowAttrManager::Load(&dlg, wxT("NewTaskDialog"), NULL);

		dlg.SetLabel(_("Edit Task"));
		dlg.m_name->SetValue( m_list->GetText(selection, 1) );

		TaskData *data = (TaskData*) m_list->GetItemData(selection);
		dlg.m_regex->SetValue(data->m_regex);
		if (dlg.ShowModal() == wxID_OK) {
			m_list->SetTextColumn(selection, 1, dlg.m_name->GetValue());
			m_list->SetItemClientData(selection, new TaskData(dlg.m_regex->GetValue()));
		}
		WindowAttrManager::Save(&dlg, wxT("NewTaskDialog"), NULL);
	}
}

void TasksFindWhatDlg::OnEditTaskUI(wxUpdateUIEvent& event)
{
	event.Enable(m_list->GetItemCount() && m_list->GetSelection() != wxNOT_FOUND);
}

void TasksFindWhatDlg::DoAddLine(const wxString& name, const wxString& regex, bool enabled)
{
	long item = m_list->AppendRow();
	m_list->SetCheckboxRow(item, enabled);
	m_list->SetTextColumn(item, 1, name);
	m_list->SetItemClientData(item, new TaskData(regex));
}

void TasksFindWhatDlg::DoSaveList()
{
	// Save all items
	TasksPanelData data;
	std::map<wxString, wxString> items;
	wxArrayString                enabledItems;

	for (int i=0; i<m_list->GetItemCount(); i++) {
		wxString name  = m_list->GetText(i, 1);
		TaskData *clientData = (TaskData*) m_list->GetItemData(i);
		items[name] = clientData->m_regex;

		if (m_list->IsChecked(i))
			enabledItems.Add(name);
	}

	data.SetEnabledItems(enabledItems);
	data.SetTasks(items);
	EditorConfigST::Get()->WriteObject(wxT("TasksPanelData"), &data);
}

void TasksFindWhatDlg::OnButtonOk(wxCommandEvent& event)
{
	DoSaveList();
	event.Skip();
}
