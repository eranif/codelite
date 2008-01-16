#include "newdlg.h"
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/stattext.h> 
#include "filepicker.h" 
#include "dirpicker.h" 
#include <wx/statline.h> 
#include <wx/msgdlg.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h> 
#include <wx/listbox.h>  
#include "project.h"
#include <map>
#include "manager.h" 
#include "editor_config.h"
#include "build_settings_config.h"

DEFINE_EVENT_TYPE(wxEVT_NEW_DLG_CREATE_LE)

NewDlg::NewDlg() 
: wxDialog()  
, m_selection(NEW_DLG_WORKSPACE)
{
}

NewDlg::NewDlg(wxWindow* parent, 
			   int selection, 
			   wxWindowID id, 
			   const wxString& caption, 
			   const wxPoint& pos, 
			   const wxSize& size, 
			   long style)
{
	Create(parent, selection, id, caption, pos, size, style);
}

bool NewDlg::Create(wxWindow* parent, 
					 int selection, 
					 wxWindowID id, 
					 const wxString& caption, 
					 const wxPoint& pos, 
					 const wxSize& size, 
					 long style)
{
	if( !wxDialog::Create(parent, id, caption, pos, size, style) )
		return false;

	m_selection = selection;

	CreateGUIControls();
	ConnectEvents();

	GetSizer()->Fit(this);
	GetSizer()->SetMinSize(500, 300);
	GetSizer()->SetSizeHints(this);
	return true;
}

NewDlg::~NewDlg()
{
}

int NewDlg::GetSelection() const 
{
	return m_selection;
}

void NewDlg::CreateGUIControls()
{
	wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);
	m_book = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(500, -1), 
								wxFNB_NO_NAV_BUTTONS | wxFNB_NO_X_BUTTON | wxFNB_NODRAG | wxFNB_FF2 |
								wxFNB_BACKGROUND_GRADIENT);

	m_book->AddPage(CreateProjectPage(), wxT("Project"), m_selection == NEW_DLG_PROJECT);
	m_book->AddPage(CreateWorkspacePage(), wxT("Workspace"), m_selection == NEW_DLG_WORKSPACE);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 5);
	
	if( m_selection == NEW_DLG_PROJECT )
	{
		m_projName->SetFocus();
	} 
	else 
	{
		m_name->SetFocus();
	}

	// Add the buttons
	m_create = new wxButton(this, wxID_OK, wxT("&Create"));
	btnSizer->Add(m_create, 0, wxALL | wxEXPAND | wxALIGN_RIGHT, 5 ); 

	m_cancel = new wxButton(this, wxID_CANCEL, wxT("Cancel"));
	btnSizer->Add(m_cancel, 0, wxALL | wxEXPAND | wxALIGN_RIGHT, 5 ); 

	mainSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND );
	mainSizer->Add(btnSizer, 0, wxALL | wxALIGN_RIGHT, 5);
}

wxWindow *NewDlg::CreateWorkspacePage()
{
	wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
	wxPanel *panel = new wxPanel(m_book);
	panel->SetSizer(panelSizer);
	panel->SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE) );

	wxStaticText* itemStaticText;
	itemStaticText = new wxStaticText( panel, wxID_STATIC, wxT("Workspace Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	panelSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	m_name = new wxTextCtrl( panel, wxID_ANY, wxT(""));
	panelSizer->Add(m_name, 0, wxALL | wxEXPAND, 5 );

	itemStaticText = new wxStaticText( panel, wxID_STATIC, wxT("Workspace Path:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	panelSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	m_pathPicker = new DirPicker(panel);
	panelSizer->Add(m_pathPicker, 0, wxEXPAND | wxALL, 5);
	return panel;
}

wxWindow *NewDlg::CreateProjectPage()
{
	wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *hSizer = new wxBoxSizer(wxHORIZONTAL);
	wxPanel *panel = new wxPanel(m_book);
	panel->SetSizer(hSizer);
	panel->SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT) );

	wxStaticText* itemStaticText;

	m_projTypes = new wxListBox(panel, wxID_ANY);

	ManagerST::Get()->GetProjectTemplateList(m_list);

	std::list<ProjectPtr>::iterator iter = m_list.begin();
	for(; iter != m_list.end(); iter++)
	{
		m_projTypes->Append((*iter)->GetName());
	}

	iter = m_list.begin();
	if( iter != m_list.end() )
	{
		m_projTypes->SetStringSelection((*iter)->GetName());
		m_projectData.m_srcProject = (*iter);
	}
	
	itemStaticText = new wxStaticText( panel, wxID_STATIC, wxT("Project Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	panelSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	m_projName = new wxTextCtrl( panel, wxID_ANY, wxT(""));
	panelSizer->Add(m_projName, 0, wxALL | wxEXPAND, 5 );

	itemStaticText = new wxStaticText( panel, wxID_STATIC, wxT("Project Path:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	panelSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );
	//by default set here the workspace path
	m_projPathPicker = new DirPicker(panel);
	m_projPathPicker->SetPath(wxGetCwd());

	panelSizer->Add(m_projPathPicker, 0, wxEXPAND | wxALL, 5);
	
	itemStaticText = new wxStaticText( panel, wxID_STATIC, wxT("Project Compiler:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	panelSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	wxArrayString choices;
	//get list of compilers from configuration file
	BuildSettingsConfigCookie cookie;
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
	while(cmp)
	{
		choices.Add(cmp->GetName());
		cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
	}

	m_choiceCmpType = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
	panelSizer->Add(m_choiceCmpType, 0, wxEXPAND | wxALL, 5);
	
	if(m_choiceCmpType->GetCount() > 0)
	{
		m_choiceCmpType->SetSelection(0);
	}

	hSizer->Add(m_projTypes, 1, wxALL | wxEXPAND, 5);
	hSizer->Add(panelSizer, 2, wxALL | wxEXPAND, 5);
	return panel;
}

void NewDlg::OnClick(wxCommandEvent & event)
{
	wxUnusedVar(event);
	// pass data from controls to the m_data
	if( m_book->GetSelection() == NEW_DLG_WORKSPACE )
	{
		m_workspaceData.m_path = m_pathPicker->GetPath();
		m_workspaceData.m_name = m_name->GetValue();

		if( m_workspaceData.m_name.Trim().IsEmpty() )
		{
			wxMessageBox(wxT("Invalid workspace name"), wxT("Error"), wxOK | wxICON_HAND);
			return;
		}

		if( !wxDirExists(m_workspaceData.m_path) )
		{
			wxMessageBox(wxT("Invalid path"), wxT("Error"), wxOK | wxICON_HAND);
			return;
		}

		wxCommandEvent event(wxEVT_NEW_DLG_CREATE_LE, GetId());
		event.SetEventObject(this);
		::wxPostEvent(GetParent()->GetEventHandler(), event);
	} 
	else if( m_book->GetSelection() == NEW_DLG_PROJECT )
	{
		m_projectData.m_name = m_projName->GetValue();
		m_projectData.m_path = m_projPathPicker->GetPath();
		m_projectData.m_cmpType = m_choiceCmpType->GetStringSelection();

		//the project type is determined according to the selected project name
		m_projectData.m_srcProject = FindProject(m_projTypes->GetStringSelection());

		if( m_projectData.m_name.Trim().IsEmpty() )
		{
			wxMessageBox(wxT("Invalid project name"), wxT("Error"), wxOK | wxICON_HAND);
			return;
		}

		if( !wxDirExists(m_projectData.m_path) )
		{
			wxMessageBox(wxT("Invalid path"), wxT("Error"), wxOK | wxICON_HAND);
			return;
		}
		wxCommandEvent event(wxEVT_NEW_DLG_CREATE_LE, GetId());
		event.SetEventObject(this);
		::wxPostEvent(GetParent()->GetEventHandler(), event);
	}
	EndModal(wxID_OK);
}

void NewDlg::OnListItemSelected(wxListEvent &event)
{
	m_projectData.m_srcProject = FindProject( event.GetText() );
}

void NewDlg::ConnectEvents()
{
	// Connect buttons
	m_create->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewDlg::OnClick), NULL, this);
	m_projTypes->Connect(wxID_ANY, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(NewDlg::OnListItemSelected), NULL, this);
}

ProjectPtr NewDlg::FindProject(const wxString &name)
{
	std::list<ProjectPtr>::iterator iter = m_list.begin();
	for(; iter != m_list.end(); iter++)
	{
		if((*iter)->GetName() == name)
		{
			return (*iter);
		}
	}
	return NULL;
}

