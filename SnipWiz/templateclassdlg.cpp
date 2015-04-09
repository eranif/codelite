//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : templateclassdlg.cpp
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

#include <wx/textbuf.h>
#include <wx/dirdlg.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include "VirtualDirectorySelectorDlg.h"
#include <wx/filefn.h>
#include "editor_config.h"
#include "project.h"
#include "templateclassdlg.h"
#include <wx/msgdlg.h>
#include "swStringDb.h"
#include "snipwiz.h"
#include "swGlobals.h"
#include "imanager.h"

static const wxString swHeader = wxT( "header" );
static const wxString swSource = wxT( "source" );
static const wxString swPhClass = wxT( "%CLASS%" );
static const wxString swNsList = wxT( "%NAMESPACELIST%" );
static const wxString swNsEndList = wxT( "%NAMESPACELISTEND%" );

TemplateClassDlg::TemplateClassDlg( wxWindow* parent, SnipWiz *plugin, IManager *manager )
		: TemplateClassBaseDlg( parent )
		, m_plugin(plugin)
		, m_pManager(manager)
{
	m_checkboxVirtualToReal->SetValue(true);
	Initialize();
}

void TemplateClassDlg::Initialize()
{
	// set tab sizes
	wxTextAttr attribs = m_textCtrlHeader->GetDefaultStyle();
	wxArrayInt	tabs = attribs.GetTabs();
	int tab = 70;
	for ( int i = 1; i < 20; i++ )
		tabs.Add( tab * i );
	attribs.SetTabs( tabs );
	m_textCtrlHeader->SetDefaultStyle( attribs );
	m_textCtrlImpl->SetDefaultStyle( attribs );

	GetStringDb()->Load( m_pluginPath + defaultTmplFile );

	wxArrayString templates;
	GetStringDb()->GetAllSets( templates );
	for ( wxUint32 i = 0; i < templates.GetCount(); i++ ) {
		m_comboxTemplates->AppendString( templates[i] );
		m_comboxCurrentTemplate->AppendString( templates[i] );
	}
	if ( templates.GetCount() ) {
		m_comboxTemplates->Select( 0 );
		wxString set = m_comboxTemplates->GetValue();
		m_textCtrlHeader->SetValue( GetStringDb()->GetString( set, swHeader ) );
		m_textCtrlImpl->SetValue( GetStringDb()->GetString( set, swSource ) );
		m_comboxCurrentTemplate->Select( 0 );
	}
	TreeItemInfo item = m_pManager->GetSelectedTreeItemInfo( TreeFileView );
	if ( item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory ) {
		m_virtualFolder = VirtualDirectorySelectorDlg::DoGetPath( m_pManager->GetTree( TreeFileView ), item.m_item, false );
	}
	m_textCtrlVD->SetValue( m_virtualFolder );
	if(m_virtualFolder.IsEmpty() == false){
		m_staticProjectTreeFolder->SetForegroundColour(wxColour(0,128,0));
	}
	m_textCtrlClassName->SetFocus();
	UpdatePath();
}

void TemplateClassDlg::OnClassNameEntered( wxCommandEvent& event )
{
	wxUnusedVar( event );
	wxString buffer = m_textCtrlClassName->GetValue();
	
	if ( buffer.IsEmpty() ) {
		m_textCtrlHeaderFile->SetValue(wxT(""));
		m_textCtrlCppFile->SetValue(wxT(""));
		return;
	} else {
		buffer = GetNsList().Last();
		m_textCtrlHeaderFile->SetValue( buffer.Lower() + wxT( ".h" ) );
		m_textCtrlCppFile->SetValue( buffer.Lower() + wxT( ".cpp" ) );
	}
}

void TemplateClassDlg::OnBrowseVD( wxCommandEvent& event )
{
	wxUnusedVar( event );
	VirtualDirectorySelectorDlg dlg( this, m_pManager->GetWorkspace(), m_textCtrlVD->GetValue() );
	if ( dlg.ShowModal() == wxID_OK ) {
		m_textCtrlVD->SetValue( dlg.GetVirtualDirectoryPath() );
		m_staticProjectTreeFolder->SetForegroundColour(wxColour(0,128,0));
		m_staticProjectTreeFolder->Refresh();
	}
	UpdatePath();
}

void TemplateClassDlg::OnBrowseFilePath( wxCommandEvent& event )
{
	wxUnusedVar( event );
	m_checkboxVirtualToReal->SetValue(false);
	
	wxString dir = wxT("");
	if ( wxFileName::DirExists( m_projectPath ) ) {
		dir = m_projectPath;
	}

	dir = wxDirSelector( _("Select output folder"), dir, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if ( !dir.IsEmpty() ) {
		m_projectPath = dir;
		m_textCtrlFilePath->SetValue(m_projectPath);
	}
}

void TemplateClassDlg::OnGenerate( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxArrayString files;
	
	if(m_textCtrlClassName->GetValue().IsEmpty())
		return;
	
	wxString namespacesList = "";
	wxString namespacesEndList = "";
	wxString namespacesEndListComment = " // ";
	wxArrayString ns = GetNsList();
	for(int n = 0; n < ns.Count() - 1; n++){
		namespacesList += wxString::Format("namespace %s{ ", ns[n]);
		namespacesEndList += "}";
		namespacesEndListComment += ns[n];
		if(n < ns.Count() - 2)
			namespacesEndListComment += "::";
	}
	namespacesEndList += namespacesEndListComment;
	
	wxString newClassName = ns.Last();
	wxString baseClass = m_comboxCurrentTemplate->GetValue();
	
	UpdatePath();
	
	if (!wxEndsWithPathSeparator(m_projectPath))
		m_projectPath += wxFILE_SEP_PATH;
		
	if (!wxFileName::DirExists( m_projectPath ) ) {
		wxFileName::Mkdir(m_projectPath);
	}
	
	wxString buffer = GetStringDb()->GetString( baseClass, swHeader );
	buffer.Replace( swNsList, namespacesList );
	buffer.Replace( swPhClass, newClassName );
	buffer.Replace(wxT("\v"), eol[m_curEol].c_str());
	buffer.Replace( swNsEndList, namespacesEndList );

	files.Add( m_projectPath + m_textCtrlHeaderFile->GetValue() );
	SaveBufferToFile( files.Item(0), buffer );

	buffer = wxString::Format( wxT( "#include \"%s\"%s%s" ), m_textCtrlHeaderFile->GetValue().c_str(), eol[m_curEol].c_str(), eol[m_curEol].c_str() );
	buffer += GetStringDb()->GetString( baseClass, swSource );
	buffer.Replace( swNsList, namespacesList );
	buffer.Replace( swPhClass, newClassName );
	buffer.Replace(wxT("\v"), eol[m_curEol].c_str());
	buffer.Replace( swNsEndList, namespacesEndList );
	files.Add( m_projectPath + m_textCtrlCppFile->GetValue() );
	
	SaveBufferToFile( files.Item(1), buffer );

	if ( !m_textCtrlVD->GetValue().IsEmpty() ) {
		// Create the Success message first, as 'files' may be altered during creation
		wxString msg;
		msg << wxString::Format( wxT( "%s%s" ), files.Item(0).c_str(), eol[m_curEol].c_str())
		<< wxString::Format( wxT( "%s%s%s" ), files.Item(1).c_str(), eol[m_curEol].c_str(), eol[m_curEol].c_str())
		<< _( "Files successfully created." );
		// We have a .cpp and an .h file, and there may well be a :src and an :include folder available
		// So try to place the files appropriately. If that fails, dump both in the selected folder
		
		bool smartAddFiles = EditorConfigST::Get()->GetOptions()->GetOptions() & OptionsConfig::Opt_SmartAddFiles;
		
		if ( (smartAddFiles && m_pManager->AddFilesToVirtualFolderIntelligently( m_textCtrlVD->GetValue(), files )) || m_pManager->AddFilesToVirtualFolder( m_textCtrlVD->GetValue(), files ) ) 
		{
			wxMessageBox(msg, _("Add template class"), wxOK|wxCENTER|wxICON_INFORMATION, this);
			EndModal(wxID_OK);
			return;
		}
	}

	wxMessageBox(_("Adding the template class failed"), _("Oops"), wxOK|wxCENTER|wxICON_WARNING, this);
	EndModal(wxID_CANCEL);	// The return value isn't actually used at present, but send Cancel on failure for future-proofing
}

void TemplateClassDlg::OnGenerateUI( wxUpdateUIEvent& event )
{
	wxUnusedVar( event );
	if ( m_comboxCurrentTemplate->GetSelection() == wxNOT_FOUND ||
	        m_textCtrlClassName->GetValue().IsEmpty() ||
	        m_textCtrlHeaderFile->GetValue().IsEmpty() ||
	        m_textCtrlCppFile->GetValue().IsEmpty() )
		event.Enable( false );
	else
		event.Enable( true );
}

void TemplateClassDlg::OnQuit( wxCommandEvent& event )
{
	wxUnusedVar(event);
	GetStringDb()->Save( m_pluginPath + defaultTmplFile );
	EndModal(wxID_CANCEL);
}

void TemplateClassDlg::OnTemplateClassSelected( wxCommandEvent& event )
{
	wxUnusedVar( event );
	wxString set = m_comboxTemplates->GetValue();
	if ( GetStringDb()->IsSet( set ) ) {
		m_textCtrlHeader->SetValue( GetStringDb()->GetString( set, swHeader ) );
		m_textCtrlImpl->SetValue( GetStringDb()->GetString( set, swSource) );
	}
}

void TemplateClassDlg::OnButtonAdd( wxCommandEvent& event )
{
	wxUnusedVar( event );
	wxString set = m_comboxTemplates->GetValue();
	bool isSet = GetStringDb()->IsSet( set );
	if ( isSet ) {
		int ret = wxMessageBox( _("Class exists!\nOverwrite?"), _("Add class"), wxYES_NO | wxICON_QUESTION );
		if ( ret == wxNO )
			return;
	}
	GetStringDb()->SetString( set, swHeader, m_textCtrlHeader->GetValue() );
	GetStringDb()->SetString( set, swSource, m_textCtrlImpl->GetValue() );
	if ( !isSet ) {
		m_comboxTemplates->AppendString( set );
	}

	RefreshTemplateList();
	m_modified = true;
}

void TemplateClassDlg::OnButtonAddUI( wxUpdateUIEvent& event )
{
	if ( m_comboxTemplates->GetValue().IsEmpty() || m_textCtrlHeader->GetValue().IsEmpty() || m_textCtrlImpl->GetValue().IsEmpty() )
		event.Enable( false );
	else
		event.Enable( true );
}

void TemplateClassDlg::OnButtonChange( wxCommandEvent& event )
{
	wxUnusedVar( event );
	wxString set = m_comboxTemplates->GetValue();
	bool isSet = GetStringDb()->IsSet( set );
	if ( !isSet ) {
		int ret = ::wxMessageBox( _("That class doesn't exist!\nTry again?"), _("Change class"), wxYES_NO | wxICON_QUESTION );
		if ( ret == wxNO )
			return;
	}
	GetStringDb()->SetString( set, swHeader, m_textCtrlHeader->GetValue() );
	GetStringDb()->SetString( set, swSource, m_textCtrlImpl->GetValue() );
	if ( !isSet )
		m_comboxTemplates->AppendString( set );
	RefreshTemplateList();
	m_modified = true;
}

void TemplateClassDlg::OnButtonChangeUI( wxUpdateUIEvent& event )
{
	if ( m_comboxTemplates->GetSelection() == wxNOT_FOUND )
		event.Enable( false );
	else
		event.Enable( true );
	if ( m_textCtrlHeader->IsModified() == false && m_textCtrlImpl->IsModified() == false )
		event.Enable( false );
}

void TemplateClassDlg::OnButtonRemove( wxCommandEvent& event )
{
	wxUnusedVar( event );
	wxString set = m_comboxTemplates->GetValue();
	bool isSet = GetStringDb()->IsSet( set );
	if ( !isSet ) {
		::wxMessageBox( wxT( "Class not found!\nNothing to remove." ), wxT( "Remove class" ) );
		return;
	}
	GetStringDb()->DeleteKey( set, swHeader );
	GetStringDb()->DeleteKey( set, swSource );
	long index = m_comboxTemplates->FindString( set );
	m_comboxTemplates->Delete( index );
	RefreshTemplateList();
	m_modified = true;
}

void TemplateClassDlg::OnButtonRemoveUI( wxUpdateUIEvent& event )
{
	if ( m_comboxTemplates->GetSelection() == wxNOT_FOUND )
		event.Enable( false );
	else
		event.Enable( true );
}

void TemplateClassDlg::OnButtonClear(wxCommandEvent& e)
{
	m_comboxTemplates->SetValue( wxT( "" ) );
	m_textCtrlImpl->SetValue( wxT( "" ) );
	m_textCtrlHeader->SetValue( wxT( "" ) );
}

void TemplateClassDlg::OnButtonClearUI(wxUpdateUIEvent& e)
{
	e.Enable(true);
}

void TemplateClassDlg::OnInsertClassKeyword( wxCommandEvent& event )
{
	wxUnusedVar( event );
	long from, to;

	if ( m_notebookFiles->GetSelection() == 0 ) {
		m_textCtrlHeader->GetSelection( &from, &to );
		m_textCtrlHeader->Replace( from, to, swPhClass );
		m_textCtrlHeader->SetFocus();
	} else {
		m_textCtrlImpl->GetSelection( &from, &to );
		m_textCtrlImpl->Replace( from, to, swPhClass );
		m_textCtrlImpl->SetFocus();
	}
}

void TemplateClassDlg::OnInsertClassKeywordUI( wxUpdateUIEvent& event )
{
	event.Enable(true);
}

void TemplateClassDlg::OnHeaderFileContentChnaged( wxCommandEvent& event )
{
	event.Skip();
}

void TemplateClassDlg::OnImplFileContentChnaged( wxCommandEvent& event )
{
	event.Skip();
}

swStringDb* TemplateClassDlg::GetStringDb()
{
	return m_plugin->GetStringDb();
}

void TemplateClassDlg::RefreshTemplateList()
{
	wxArrayString templates;
	GetStringDb()->GetAllSets( templates );
	m_comboxCurrentTemplate->Clear();

	for ( wxUint32 i = 0; i < templates.GetCount(); i++ ) {
		m_comboxCurrentTemplate->AppendString( templates[i] );
	}

	if ( templates.GetCount() ) {
		m_comboxCurrentTemplate->Select( 0 );
	}
}

bool TemplateClassDlg::SaveBufferToFile( const wxString filename, const wxString buffer, int eolType)
{
	wxTextFile file( filename );
	wxTextFileType tft = wxTextFileType_Dos;
	if ( file.Exists() ) {
		int ret = wxMessageBox( _( "File already exists!\n\n Overwrite?" ), _( "Generate class files" ), wxYES_NO | wxYES_DEFAULT | wxICON_EXCLAMATION );
		if ( ret == wxID_NO )
			return false;
	}
	switch ( m_curEol ) {
	case 0:
		tft = wxTextFileType_Dos;
		break;
	case 1:
		tft = wxTextFileType_Mac;
		break;
	case 2:
		tft = wxTextFileType_Unix;
		break;
	}
	file.Create();
	file.AddLine( buffer, tft );
	file.Write( tft );
	file.Close();
	return true;
}

void TemplateClassDlg::UpdatePath()
{
	TreeItemInfo item = m_pManager->GetSelectedTreeItemInfo( TreeFileView );
	if ( item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory ) {
		m_virtualFolder = VirtualDirectorySelectorDlg::DoGetPath( m_pManager->GetTree( TreeFileView ), item.m_item, false );
		m_projectPath =  item.m_fileName.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR );
	}
	
	if (m_checkboxVirtualToReal->GetValue()){
		const wxString separator = ":";
		wxString vPath = m_textCtrlVD->GetValue();
		int colon = vPath.First(separator) + 1;
		wxString extPath = vPath.SubString(colon, vPath.Length());
		extPath.Replace(separator, wxFILE_SEP_PATH);
		m_projectPath += extPath + wxFILE_SEP_PATH;
	}
	m_textCtrlFilePath->SetValue( m_projectPath );
}

void TemplateClassDlg::OnPathUpdate(wxCommandEvent& e)
{
	UpdatePath();
}

void TemplateClassDlg::OnInsertNsEndKeyword(wxCommandEvent& event)
{
	wxUnusedVar( event );
	long from, to;

	if ( m_notebookFiles->GetSelection() == 0 ) {
		m_textCtrlHeader->GetSelection( &from, &to );
		m_textCtrlHeader->Replace( from, to, swNsEndList );
		m_textCtrlHeader->SetFocus();
	} else {
		m_textCtrlImpl->GetSelection( &from, &to );
		m_textCtrlImpl->Replace( from, to, swNsEndList );
		m_textCtrlImpl->SetFocus();
	}
}

void TemplateClassDlg::OnInsertNsEndKeywordUI(wxUpdateUIEvent& event)
{
	event.Enable(true);
}

void TemplateClassDlg::OnInsertNsKeyword(wxCommandEvent& event)
{
	wxUnusedVar( event );
	long from, to;

	if ( m_notebookFiles->GetSelection() == 0 ) {
		m_textCtrlHeader->GetSelection( &from, &to );
		m_textCtrlHeader->Replace( from, to, swNsList );
		m_textCtrlHeader->SetFocus();
	} else {
		m_textCtrlImpl->GetSelection( &from, &to );
		m_textCtrlImpl->Replace( from, to, swNsList );
		m_textCtrlImpl->SetFocus();
	}
}

void TemplateClassDlg::OnInsertNsKeywordUI(wxUpdateUIEvent& event)
{
	event.Enable(true);
}

wxArrayString TemplateClassDlg::GetNsList()
{
	wxArrayString ns;
	ns.Clear();
	
	if(m_textCtrlClassName->GetValue().IsEmpty())
		return ns;
	
	wxStringTokenizer strTok(m_textCtrlClassName->GetValue(), "::", wxTOKEN_STRTOK);
	while(strTok.HasMoreTokens()){
		ns.Add(strTok.GetNextToken());
	}
	return ns;
}
