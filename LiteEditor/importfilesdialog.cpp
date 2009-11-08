//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : importfilesdialog.cpp
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


#include "importfilesdialog.h"
#include <wx/busyinfo.h>
#include <wx/dirdlg.h>
#include "manager.h"
#include "project.h"
#include "ctags_manager.h"
#include "checkdirtreectrl.h"
#include "editor_config.h"
#include "importfilessettings.h"
#include "windowattrmanager.h"

///////////////////////////////////////////////////////////////////////////

ImportFilesDialog::ImportFilesDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	ImportFilesSettings options;
	if(!EditorConfigST::Get()->ReadObject(wxT("import_dir_options"), &options)){
		//first time, read the settings from the ctags options
		options.SetFileMask( TagsManagerST::Get()->GetCtagsOptions().GetFileSpec() );
		bool noExt = TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_PARSE_EXT_LESS_FILES ? true : false;
		size_t flags(0);
		if(noExt){
			flags |= IFS_INCLUDE_FILES_WO_EXT;
		}
		options.SetFlags(flags);
	}

	if(options.GetBaseDir().IsEmpty()) {
		ProjectPtr proj = ManagerST::Get()->GetProject(ManagerST::Get()->GetActiveProjectName());
		options.SetBaseDir( proj->GetFileName().GetPath() );
	}

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Change root directory..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer3->Add( m_staticText2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_button3 = new wxButton( this, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_button3, 0, wxALL, 5 );

	mainSizer->Add( bSizer3, 0, wxEXPAND, 5 );

	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	m_treeCtrlDir = new CheckDirTreeCtrl(this, options.GetBaseDir());
	mainSizer->Add( m_treeCtrlDir, 1, wxALL|wxEXPAND, 5 );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Files extension to import:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	mainSizer->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );

	m_textCtrSpec = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_textCtrSpec, 0, wxALL|wxEXPAND, 5 );
	m_textCtrSpec->SetValue( options.GetFileMask() );

	m_checkBoxFilesWOExt = new wxCheckBox( this, wxID_ANY, _("Import files without extension as well"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxFilesWOExt->SetValue( options.GetFlags() & IFS_INCLUDE_FILES_WO_EXT ? true : false );

	mainSizer->Add( m_checkBoxFilesWOExt, 0, wxALL|wxEXPAND, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOk = new wxButton( this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault();
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );

	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );

	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );

	this->Centre( wxBOTH );

	WindowAttrManager::Load(this, wxT("ImportFilesDialog"), NULL);

	// Connect Events
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImportFilesDialog::OnChangeDir ), NULL, this );
}

ImportFilesDialog::~ImportFilesDialog()
{
	WindowAttrManager::Save(this, wxT("ImportFilesDialog"), NULL);
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImportFilesDialog::OnChangeDir ), NULL, this );

	ImportFilesSettings options;
	options.SetBaseDir(m_treeCtrlDir->GetRootDir());
	options.SetFileMask(m_textCtrSpec->GetValue());

	size_t flags(0);
	if(m_checkBoxFilesWOExt->IsChecked()) flags |= IFS_INCLUDE_FILES_WO_EXT;
	options.SetFlags( flags );
	EditorConfigST::Get()->ReadObject(wxT("import_dir_options"), &options);
}

void ImportFilesDialog::OnChangeDir(wxCommandEvent& event)
{
	wxUnusedVar(event);
	wxString path(m_treeCtrlDir->GetRootDir());
	wxString new_path = wxDirSelector(wxT("Select Directory:"), path, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);

	wxWindowDisabler disableAll;
	wxBusyInfo info(wxT("Loading directories, please wait..."), this);
	wxTheApp->Yield();
	if(new_path.IsEmpty() == false){
		m_treeCtrlDir->BuildTree( new_path );
	}
}

void ImportFilesDialog::GetDirectories(wxArrayString &dirs)
{
	m_treeCtrlDir->GetSelectedDirs(dirs);
}

bool ImportFilesDialog::ExtlessFiles()
{
	return m_checkBoxFilesWOExt->IsChecked();
}

wxString ImportFilesDialog::GetFileMask()
{
	return m_textCtrSpec->GetValue();
}

wxString ImportFilesDialog::GetBaseDir()
{
	return m_treeCtrlDir->GetRootDir();
}
