#include "open_resource_dialog.h"
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include "editor_config.h"
#include "ieditor.h"
#include "ctags_manager.h"
#include <wx/filefn.h>
#include "project.h"
#include "workspace.h"
#include <wx/wupdlock.h>
#include "imanager.h"
#include "windowattrmanager.h"
#include <vector>

wxString OpenResourceDialog::TYPE_WORKSPACE_FILE = wxT("Workspace file");
wxString OpenResourceDialog::TYPE_CLASS          = wxT("Class, struct or union");
wxString OpenResourceDialog::TYPE_MACRO          = wxT("Macro");
wxString OpenResourceDialog::TYPE_FUNCTION       = wxT("Function");
wxString OpenResourceDialog::TYPE_TYPEDEF        = wxT("Typedef");
wxString OpenResourceDialog::TYPE_NAMESPACE      = wxT("Namespace");

BEGIN_EVENT_TABLE(OpenResourceDialog, OpenResourceDialogBase)
	EVT_TIMER(XRCID("OR_TIMER"), OpenResourceDialog::OnTimer)
END_EVENT_TABLE()

OpenResourceDialog::OpenResourceDialog( wxWindow* parent, IManager *manager, const wxString &type, bool allowChangeType )
		: OpenResourceDialogBase( parent )
		, m_manager(manager)
		, m_type(type)
		, m_needRefresh(false)
{
	
	m_timer = new wxTimer(this, XRCID("OR_TIMER"));
	m_timer->Start(500);
	
	m_listOptions->InsertColumn(0, wxT(""));
	m_listOptions->InsertColumn(1, wxT(""));
	m_listOptions->InsertColumn(2, wxT(""));
	
	m_listOptions->SetColumnWidth(0, 150);
	m_listOptions->SetColumnWidth(1, 300);
	m_listOptions->SetColumnWidth(2, 300);
	
	m_textCtrlResourceName->SetFocus();
	SetLabel(wxString::Format(wxT("Open %s"), m_type.c_str()));

	WindowAttrManager::Load(this, wxT("OpenResourceDialog"), m_manager->GetConfigTool());
	SimpleLongValue l;
	l.SetValue(m_checkBoxUsePartialMatching->IsChecked() ? 1 : 0);
	m_manager->GetConfigTool()->ReadObject(wxT("OpenResourceAllowsPartialMatch"), &l);
	m_checkBoxUsePartialMatching->SetValue(l.GetValue() == 1);

	m_choiceResourceType->SetStringSelection(m_type);

	if (!allowChangeType)
		m_choiceResourceType->Enable(false);

	//load all files from the workspace
	if ( m_manager->IsWorkspaceOpen() ) {
		wxArrayString projects;
		m_manager->GetWorkspace()->GetProjectList( projects );

		for ( size_t i=0; i<projects.GetCount(); i++ ) {
			std::vector<wxFileName> fileNames;
			wxString errmsg;
			ProjectPtr p = m_manager->GetWorkspace()->FindProjectByName(projects.Item(i), errmsg);

			if ( p ) {
				p->GetFiles(fileNames, true);

				//convert std::vector to wxArrayString
				for ( std::vector<wxFileName>::iterator it = fileNames.begin(); it != fileNames.end(); it ++ ) {
					m_files.Add ( ( *it ).GetFullPath() );
				}
			}
		}
	}
	
	m_listOptions->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( OpenResourceDialog::OnItemActivated ), NULL, this );
	m_listOptions->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( OpenResourceDialog::OnItemSelected ), NULL, this );
}

OpenResourceDialog::~OpenResourceDialog()
{
	m_timer->Stop();
	delete m_timer;
	
	WindowAttrManager::Save(this, wxT("OpenResourceDialog"), m_manager->GetConfigTool());
	SimpleLongValue l;
	l.SetValue(m_checkBoxUsePartialMatching->IsChecked() ? 1 : 0);
	m_manager->GetConfigTool()->WriteObject(wxT("OpenResourceAllowsPartialMatch"), &l);
}

void OpenResourceDialog::OnText( wxCommandEvent& event )
{
	m_needRefresh = true;
	event.Skip();
}

void OpenResourceDialog::OnType( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_type = m_choiceResourceType->GetStringSelection();
	DoPopulateList();
	m_textCtrlResourceName->SetFocus();

	SetLabel(wxString::Format(wxT("Open %s"), m_type.c_str()));
}

void OpenResourceDialog::OnUsePartialMatching( wxCommandEvent& event )
{
	wxUnusedVar(event);
	DoPopulateList();
	m_textCtrlResourceName->SetFocus();
}

void OpenResourceDialog::OnEnter(wxCommandEvent& event)
{
	wxUnusedVar(event);
	int sel = m_listOptions->GetFirstSelected();
	if (sel != wxNOT_FOUND) {
		OpenResourceDialogItemData *data = (OpenResourceDialogItemData *) m_listOptions->GetItemData(sel);
		if (data) {
			m_selection = *data;
			EndModal(wxID_OK);
		}
	}
}

void OpenResourceDialog::OnItemActivated(wxListEvent& event)
{
	int sel = event.m_itemIndex;
	if (sel != wxNOT_FOUND) {
		OpenResourceDialogItemData *data = (OpenResourceDialogItemData *) m_listOptions->GetItemData(sel);
		if (data) {
			m_selection = *data;
			EndModal(wxID_OK);
		}
	}
}

void OpenResourceDialog::DoPopulateList()
{
	m_tags.clear();
	wxWindowUpdateLocker locker(this);
	wxArrayString kind;
	Clear();
	if (m_type == TYPE_WORKSPACE_FILE) {
		DoPopulateWorkspaceFile();
		return;

	} else if (m_type == TYPE_CLASS) {

		kind.Add(wxT("class"));
		kind.Add(wxT("struct"));
		kind.Add(wxT("union"));

	} else if ( m_type == TYPE_FUNCTION ) {
		kind.Add(wxT("function"));
		kind.Add(wxT("prototype"));

	} else if ( m_type == TYPE_MACRO) {
		kind.Add(wxT("macro"));

	} else if ( m_type == TYPE_TYPEDEF) {
		kind.Add(wxT("typedef"));

	} else if ( m_type == TYPE_NAMESPACE) {
		kind.Add(wxT("namespace"));

	}

	if (kind.IsEmpty() == false)
		m_manager->GetTagsManager()->GetTagsByKindLimit(m_tags, kind, 150, m_textCtrlResourceName->GetValue());

	DoPopulateTags();
}

void OpenResourceDialog::DoPopulateTags()
{
	if (m_tags.empty())
		return;

	bool gotExactMatch(false);

	wxArrayString tmpArr;
	wxString curSel = m_textCtrlResourceName->GetValue();
	wxString curSelNoStar;
	if (!curSel.Trim().Trim(false).IsEmpty()) {

		curSel = curSel.MakeLower().Trim().Trim(false);
		curSelNoStar = curSel.c_str();

		for (size_t i=0; i<m_tags.size(); i++) {
			TagEntryPtr tag = m_tags.at(i);
			wxString    name(tag->GetName());

			name.MakeLower();

			//append wildcard at the end
			if (!curSel.EndsWith(wxT("*"))) {
				curSel << wxT("*");
			}

			// FR# [2008133]
			if (m_checkBoxUsePartialMatching->IsChecked() && !curSel.StartsWith(wxT("*"))) {
				curSel.Prepend(wxT("*"));
			}

			if (wxMatchWild(curSel, name)) {
				
				// keep the fullpath
				int index(0);
				if(tag->GetKind() == wxT("function") || tag->GetKind() == wxT("prototype"))
					index = DoAppendLine(tag->GetName(), 
										 tag->GetSignature(), 
										 tag->GetScope(), 
										 tag->GetKind() == wxT("function"),
										 new OpenResourceDialogItemData(tag->GetFile(), tag->GetLine(), tag->GetPattern(), m_type, tag->GetName(), tag->GetScope()));
				else 
					index = DoAppendLine(tag->GetName(), 
										 tag->GetScope(), 
										 wxT(""), 
										 false,
										 new OpenResourceDialogItemData(tag->GetFile(), tag->GetLine(), tag->GetPattern(), m_type, tag->GetName(), tag->GetScope()));
										 
				if (curSelNoStar == name && !gotExactMatch) {
					gotExactMatch = true;
					DoSelectItem(index);
				}
			}
		}
	}

	if (m_listOptions->GetItemCount() == 150) {
		m_staticTextErrorMessage->SetLabel(wxT("Too many matches, please narrow down your search"));
	}

	if (!gotExactMatch && m_listOptions->GetItemCount()) {
		DoSelectItem(0);
	}
}

void OpenResourceDialog::DoPopulateWorkspaceFile()
{
	wxArrayString tmpArr;
	wxString curSel = m_textCtrlResourceName->GetValue();
	if (!curSel.Trim().Trim(false).IsEmpty()) {

		curSel = curSel.MakeLower().Trim().Trim(false);

		for (size_t i=0; i<m_files.GetCount(); i++) {
			wxString fileName(m_files.Item(i));
			wxString fileNameOnly(wxFileName(fileName).GetFullName());

			fileName= fileName.MakeLower();

			//append wildcard at the end
			if (!curSel.EndsWith(wxT("*"))) {
				curSel << wxT("*");
			}

			// FR# [2008133]
			if (m_checkBoxUsePartialMatching->IsChecked() && !curSel.StartsWith(wxT("*"))) {
				curSel.Prepend(wxT("*"));
			}

			if (wxMatchWild(curSel, fileNameOnly)) {
				// keep the fullpath
				tmpArr.Add(m_files.Item(i));
			}
		}
	}

	// Change was done, update the file list
	for (size_t i=0; i<tmpArr.GetCount(); i++) {
		wxFileName fn(tmpArr.Item(i));
		DoAppendLine(fn.GetFullName(), fn.GetFullPath(), wxT(""), false, new OpenResourceDialogItemData(tmpArr.Item(i), -1, wxT(""), OpenResourceDialog::TYPE_WORKSPACE_FILE, wxT(""), wxT("")));
		
		if( i == 150 ) {
			m_staticTextErrorMessage->SetLabel(wxT("Too many matches, please narrow down your search"));
			break;
		}
	}

	if (m_listOptions->GetItemCount() > 0) {
		DoSelectItem(0);
	}
}

void OpenResourceDialog::Clear()
{
	// list control does not own the client data, we need to free it ourselves
	for(int i=0; i<m_listOptions->GetItemCount(); i++){
		OpenResourceDialogItemData *data = (OpenResourceDialogItemData *) m_listOptions->GetItemData(i);
		if (data) {
			delete data;
		}
	}
	m_listOptions->DeleteAllItems();
	m_staticTextErrorMessage->SetLabel(wxT(""));
	m_fullText->SetLabel(wxT(""));
}

void OpenResourceDialog::OpenSelection(const OpenResourceDialogItemData& selection, IManager* manager)
{
	if ( manager && manager->OpenFile(selection.m_file, wxEmptyString, selection.m_line) ) {
		IEditor *editor = manager->GetActiveEditor();
		if ( editor && selection.m_name.IsEmpty() == false) {
			editor->FindAndSelect(selection.m_pattern, selection.m_name, 0, manager->GetNavigationMgr());
		}
	}
}

void OpenResourceDialog::OnKeyDown(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_DOWN && m_listOptions->GetItemCount()> 0) {
		//up key
		int cursel = m_listOptions->GetFirstSelected();
		if (cursel != wxNOT_FOUND) {
			//there is a selection in the listbox
			cursel++;
			if (cursel >= (int)m_listOptions->GetItemCount()) {
				//already at last item, cant scroll anymore
				return;
			}
			DoSelectItem(cursel);

		} else {
			//no selection is made
			DoSelectItem(0);
		}
		return;

	} else if (event.GetKeyCode() == WXK_UP && m_listOptions->GetItemCount() > 0) {
		//up key
		int cursel = m_listOptions->GetFirstSelected();
		if (cursel != wxNOT_FOUND) {
			//there is a selection in the listbox
			cursel--;
			if (cursel < 0) {
				//already at first item, cant scroll anymore
				return;
			}
			DoSelectItem(cursel);

		} else {
			//no selection is made
			DoSelectItem(0);
		}
		return;

	} else
		event.Skip();
}

void OpenResourceDialog::OnOK(wxCommandEvent& event)
{
	event.Skip();
}

void OpenResourceDialog::OnOKUI(wxUpdateUIEvent& event)
{
	event.Enable(m_selection.IsOk());
}


bool OpenResourceDialogItemData::IsOk() const
{
	if (m_resourceType == OpenResourceDialog::TYPE_WORKSPACE_FILE) {
		return m_file.IsEmpty() == false;
	} else {
		// tag
		return m_file.IsEmpty() == false && m_name.IsEmpty() == false;
	}
}

void OpenResourceDialog::DoSelectItem(int selection, bool makeFirst)
{
	//no selection is made
	m_listOptions->Select(selection);
	if(makeFirst)
		m_listOptions->EnsureVisible(selection);
		
	// display the full name at the bottom static text control
	OpenResourceDialogItemData *data = (OpenResourceDialogItemData *) m_listOptions->GetItemData(selection);
	m_selection = *data;
	m_fullText->SetLabel(data->m_file);
}

void OpenResourceDialog::OnItemSelected(wxListEvent& event)
{
	event.Skip();
	if(event.m_itemIndex != wxNOT_FOUND) {
		// display the full name at the bottom static text control
		OpenResourceDialogItemData *data = (OpenResourceDialogItemData *) m_listOptions->GetItemData(event.m_itemIndex );
		m_selection = *data;
		m_fullText->SetLabel(data->m_file);
	}
}

int OpenResourceDialog::DoAppendLine(const wxString& col1, const wxString& col2, const wxString &col3, bool boldFont, OpenResourceDialogItemData* clientData)
{
	int index = AppendListCtrlRow(m_listOptions);
	SetColumnText(m_listOptions, index, 0, col1);
	SetColumnText(m_listOptions, index, 1, col2);
	SetColumnText(m_listOptions, index, 2, col3);
	m_listOptions->SetItemPtrData(index, (wxUIntPtr)(clientData));
	
	// Mark implementations with bold font
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if(boldFont)
		font.SetWeight(wxBOLD);
	m_listOptions->SetItemFont(index, font);
	return index;
}

void OpenResourceDialog::OnTimer(wxTimerEvent& event)
{
	if(m_needRefresh)
		DoPopulateList();
		
	m_needRefresh = false;
}
