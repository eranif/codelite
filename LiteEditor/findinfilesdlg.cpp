//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : findinfilesdlg.cpp              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "search_thread.h"
#include "dirpicker.h"
#include "manager.h"
#include "frame.h"
#include "macros.h"
#include "findinfilesdlg.h"
#include "findresultstab.h"
#include "replaceinfilespanel.h"


BEGIN_EVENT_TABLE(FindInFilesDialog, wxDialog)
    EVT_CLOSE(FindInFilesDialog::OnClose)
    EVT_CHAR_HOOK(FindInFilesDialog::OnCharEvent)
END_EVENT_TABLE()

FindInFilesDialog::FindInFilesDialog(wxWindow* parent, wxWindowID id, const FindReplaceData& data, size_t numpages)
    : wxDialog(parent, id, wxT("Find In Files"), wxDefaultPosition, wxSize(450, 200)) 
    , m_data(data)
{
	CreateGUIControls(numpages);
	ConnectEvents();

	GetSizer()->Fit(this);
	GetSizer()->SetMinSize(wxSize(600, 300));
	GetSizer()->SetSizeHints(this);
}

FindInFilesDialog::~FindInFilesDialog()
{
}

void FindInFilesDialog::CreateGUIControls(size_t numpages)
{
	wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	wxStaticText* itemStaticText;
	itemStaticText = new wxStaticText( this, wxID_STATIC, _("Find What:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mainSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	m_findString = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
	m_findString->Clear();
	m_findString->Append(m_data.GetFindStringArr());
	m_findString->SetValue(m_data.GetFindString());
	mainSizer->Add(m_findString, 0, wxALL | wxEXPAND, 5 );

	itemStaticText = new wxStaticText( this, wxID_STATIC, _("Look In:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mainSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	m_dirPicker = new DirPicker(this, wxID_ANY, wxT("..."), wxEmptyString, wxT("Select a folder:"), wxDefaultPosition, wxDefaultSize, wxDP_USE_COMBOBOX);
	mainSizer->Add(m_dirPicker, 0, wxEXPAND | wxALL, 5);
	
	wxArrayString choices;
	choices.Add(SEARCH_IN_PROJECT);
	choices.Add(SEARCH_IN_WORKSPACE);
	choices.Add(SEARCH_IN_CURR_FILE_PROJECT);
	m_dirPicker->SetValues(choices, 1);

	// Add the options
	wxStaticBoxSizer *sz = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Options:"));
	mainSizer->Add(sz, 1, wxEXPAND | wxALL, 5);

	m_matchCase = new wxCheckBox(this, wxID_ANY, wxT("&Match case"));
	m_matchCase->SetValue(m_data.GetFlags() & wxFRD_MATCHCASE ? true : false);
	sz->Add(m_matchCase, 1, wxALL | wxEXPAND, 5 );

	m_matchWholeWord = new wxCheckBox(this, wxID_ANY, wxT("Match &whole word"));
	m_matchWholeWord->SetValue(m_data.GetFlags() & wxFRD_MATCHWHOLEWORD ? true : false);
	sz->Add(m_matchWholeWord, 1, wxALL | wxEXPAND, 5 );

	m_regualrExpression = new wxCheckBox(this, wxID_ANY, wxT("Regular &expression"));
	m_regualrExpression->SetValue(m_data.GetFlags() & wxFRD_REGULAREXPRESSION ? true : false);
	sz->Add(m_regualrExpression, 1, wxALL | wxEXPAND, 5 );

	m_fontEncoding = new wxCheckBox(this, wxID_ANY, wxT("Use the editor's font encoding (when left unchecked encoding is set to UTF8)"));
	m_fontEncoding->SetValue(m_data.GetFlags() & wxFRD_USEFONTENCODING ? true : false);
	sz->Add(m_fontEncoding, 1, wxALL | wxEXPAND, 5 );
	
    m_printScope = new wxCheckBox(this, wxID_ANY, wxT("Display C++ scope (class/function) in result match"));
    m_printScope->SetValue(m_data.GetFlags() & wxFRD_DISPLAYSCOPE ? true : false);
    sz->Add(m_printScope, 1, wxALL | wxEXPAND, 5 );

	itemStaticText = new wxStaticText( this, wxID_STATIC, wxT("Look at these file &types:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	sz->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	wxString options [] = {
			wxT("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc"), 
			wxT("*.*") };

	m_fileTypes = new wxComboBox(this, 
								wxID_ANY, 
								options[0], 
								wxDefaultPosition, wxDefaultSize,
								2, options, wxCB_DROPDOWN);
	sz->Add(m_fileTypes, 0, wxEXPAND | wxALL, 5);

	itemStaticText = new wxStaticText( this, wxID_STATIC, wxT("Display search results in tab:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	sz->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );
	
	wxArrayString tabs;
    for (size_t n = 1; n <= numpages; n++) {
        tabs.Add(wxString::Format(wxT("Find Results %u"), n));
    }	
	m_searchResultsTab = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, tabs);
	sz->Add(m_searchResultsTab, 0, wxEXPAND | wxALL, 5 );
	m_searchResultsTab->SetSelection(0);
	
	// Add the buttons
	m_find = new wxButton(this, wxID_ANY, wxT("&Find"));
	btnSizer->Add(m_find, 1, wxALL | wxEXPAND, 5 ); 

	m_replaceAll = new wxButton(this, wxID_ANY, wxT("Find &Replace Candidates"));
	btnSizer->Add(m_replaceAll, 1, wxALL | wxEXPAND, 5 ); 

	m_stop = new wxButton(this, wxID_ANY, wxT("&Stop Search"));
	btnSizer->Add(m_stop, 1, wxALL | wxEXPAND, 5 ); 

	m_cancel = new wxButton(this, wxID_ANY, wxT("Close"));
	btnSizer->Add(m_cancel, 1, wxALL | wxEXPAND, 5 ); 

	mainSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND );
	mainSizer->Add(btnSizer, 0, wxEXPAND|wxALL, 5);
}

void FindInFilesDialog::SetRootDir(const wxString &rootDir)
{
    m_dirPicker->SetPath(rootDir);
}

void FindInFilesDialog::DoSearchReplace()
{
	SearchData data = DoGetSearchData();
    data.SetOwner(Frame::Get()->GetOutputPane()->GetReplaceResultsTab());
	SearchThreadST::Get()->PerformSearch(data);
	Hide();
}

void FindInFilesDialog::DoSearch()
{
	SearchData data = DoGetSearchData();
    data.SetOwner(Frame::Get()->GetOutputPane()->GetFindResultsTab());
	SearchThreadST::Get()->PerformSearch(data);
	Hide();
}

void FindInFilesDialog::OnClick(wxCommandEvent &event)
{
	wxObject *btnClicked = event.GetEventObject();
	size_t flags = m_data.GetFlags();
	m_data.SetFindString( m_findString->GetValue() );

	if(btnClicked == m_stop){
		SearchThreadST::Get()->StopSearch();
	} else if(btnClicked == m_find){
		DoSearch();		
	} else if(btnClicked == m_replaceAll){
		DoSearchReplace();
	} else if(btnClicked == m_cancel){
		// Hide the dialog
		Hide();
	} else if(btnClicked == m_matchCase){
		if(m_matchCase->IsChecked()) {
			flags |= wxFRD_MATCHCASE;
		} else {
			flags &= ~(wxFRD_MATCHCASE);
		}
	} else if(btnClicked == m_matchWholeWord){
		if(m_matchWholeWord->IsChecked()) {
			flags |= wxFRD_MATCHWHOLEWORD;
		} else {
			flags &= ~(wxFRD_MATCHWHOLEWORD);
		}
	} else if(btnClicked == m_regualrExpression){
		if(m_regualrExpression->IsChecked()) {
			flags |= wxFRD_REGULAREXPRESSION;
		} else {
			flags &= ~(wxFRD_REGULAREXPRESSION);
		}
	} else if(btnClicked == m_fontEncoding){
		if(m_fontEncoding->IsChecked()) {
			flags |= wxFRD_USEFONTENCODING;
		} else {
			flags &= ~(wxFRD_USEFONTENCODING);
		}
    } else if(btnClicked == m_printScope){
		if(m_printScope->IsChecked()) {
			flags |= wxFRD_DISPLAYSCOPE;
		} else {
			flags &= ~(wxFRD_DISPLAYSCOPE);
		}
	}

	// Set the updated flags
	m_data.SetFlags(flags);
}

void FindInFilesDialog::OnClose(wxCloseEvent &e)
{
	wxUnusedVar(e);
	Hide();
}

void FindInFilesDialog::ConnectEvents()
{
	// Connect buttons
	m_find->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);
	m_replaceAll->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);
	m_cancel->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);
	m_stop->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);

	// connect options
	m_matchCase->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);
	m_matchWholeWord->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);
	m_regualrExpression->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);
	m_fontEncoding->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);
    m_printScope->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);
}

void FindInFilesDialog::OnCharEvent(wxKeyEvent &event)
{
	if(event.GetKeyCode() == WXK_ESCAPE){
		Hide();
		return;
	} 
	else if(event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER){
		m_data.SetFindString( m_findString->GetValue() );
		DoSearch();
		return;
	}
	event.Skip();
}

bool FindInFilesDialog::Show()
{
    bool res = IsShown() || wxDialog::Show();
    if (res) {
        LEditor *editor = Frame::Get()->GetMainBook()->GetActiveEditor();
        if (editor) {
            //if we have an open editor, and a selected text, make this text the search string
            wxString selText = editor->GetSelectedText();
            if (!selText.IsEmpty()) {
                m_findString->SetValue(selText);
            }
        }
        m_findString->SetSelection(-1, -1); // select all
        m_findString->SetFocus();
    }
	return res;
}

SearchData FindInFilesDialog::DoGetSearchData()
{
	SearchData data;
	wxString findStr(m_data.GetFindString());
	if(m_findString->GetValue().IsEmpty() == false){
		findStr = m_findString->GetValue();
	}

	data.SetFindString(findStr);
	data.SetMatchCase( (m_data.GetFlags() & wxFRD_MATCHCASE) != 0);
	data.SetMatchWholeWord((m_data.GetFlags() & wxFRD_MATCHWHOLEWORD) != 0);
	data.SetRegularExpression((m_data.GetFlags() & wxFRD_REGULAREXPRESSION) != 0);
	data.SetRootDir(m_dirPicker->GetPath());
	data.SetUseEditorFontConfig((m_data.GetFlags() & wxFRD_USEFONTENCODING) != 0);
    data.SetDisplayScope((m_data.GetFlags() & wxFRD_DISPLAYSCOPE) != 0);
    
	if(m_dirPicker->GetPath() == SEARCH_IN_WORKSPACE){

		wxArrayString files;
		ManagerST::Get()->GetWorkspaceFiles(files);
		data.SetFiles(files);
		
	}else if(m_dirPicker->GetPath() == SEARCH_IN_PROJECT){
		
		wxArrayString files;
		ManagerST::Get()->GetProjectFiles(ManagerST::Get()->GetActiveProjectName(), files);
		data.SetFiles(files);
		
	}else if(m_dirPicker->GetPath() == SEARCH_IN_CURR_FILE_PROJECT){
		
		wxArrayString files;
		wxString project = ManagerST::Get()->GetActiveProjectName();
		
		if(ManagerST::Get()->GetActiveEditor()){
			// use the active file's project
			wxFileName activeFile = ManagerST::Get()->GetActiveEditor()->GetFileName();
			project = ManagerST::Get()->GetProjectNameByFile(activeFile.GetFullPath());
		}
		
		ManagerST::Get()->GetProjectFiles(project, files);
		data.SetFiles(files);
	}
	
	data.SetOutputTab( m_searchResultsTab->GetSelection() );
	data.SetExtensions(m_fileTypes->GetValue());
	return data;
}

