/////////////////////////////////////////////////////////////////////////////
// Name:        snipwiz.cpp
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     09/02/08
// RCS-ID:
// Copyright:   2008 Frank Lichtner
// Licence:		GNU General Public Licence
/////////////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/filename.h>
#include "templateclassdlg.h"
#include <wx/wfstream.h>
#include <wx/textdlg.h>
#include <wx/numdlg.h>
#include <wx/clipbrd.h>
#include "editsnippetsdlg.h"
#include "swGlobals.h"
#include "snipwiz.h"
#include "wxSerialize.h"
#include "workspace.h"
//------------------------------------------------------------

#define FRLSNIPWIZ_VERSION 1000
#define FRLSNIPWIZ_HEADER _T("Code snippet wizard file")

#define CARET		wxT("@")
#define SELECTION	wxT("$")
////------------------------------------------------------------
static SnipWiz* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin( IManager *manager )
{
	if ( thePlugin == 0 ) {
		thePlugin = new SnipWiz( manager );
	}
	return thePlugin;
}
//------------------------------------------------------------

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor( wxT( "Frank Lichtner" ) );
	info.SetName( plugName );
	info.SetDescription( wxT( "A small tool to add expandable code snippets and template classes" ) );
	info.SetVersion( wxT( "v1.0" ) );
	return info;
}
//------------------------------------------------------------
extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

//------------------------------------------------------------
// When creating new file, we default the EOL mode to 
// the OS conventions
static int GetEOLByOS()
{
#if defined(__WXMAC__)
	return 1/*wxSCI_EOL_CR*/;
#elif defined(__WXGTK__)
	return 2/*wxSCI_EOL_LF*/;
#else
	return 0/*wxSCI_EOL_CRLF*/;
#endif
}

//------------------------------------------------------------
SnipWiz::SnipWiz( IManager *manager )
		: IPlugin( manager )
{
	m_topWin = NULL;
	m_longName = wxT( "Snippet wizard" );
	m_shortName = plugName;
	m_sepItem = NULL;
	m_topWin = wxTheApp;
	// get plugin path
	m_pluginPath = m_mgr->GetStartupDirectory();
	m_pluginPath += wxFILE_SEP_PATH;
	m_pluginPath += wxT( "templates" );
	m_pluginPath += wxFILE_SEP_PATH;
	if ( ! wxFileName::DirExists(m_pluginPath) ){
		wxFileName::Mkdir(m_pluginPath);
	}
  
	m_StringDb.SetCompress( true );

	m_StringDb.Load( m_pluginPath + defaultTmplFile );

	m_StringDb.GetAllSnippetKeys( m_snippets );
	if ( !m_snippets.GetCount() ) {
		IntSnippets();
		m_StringDb.GetAllSnippetKeys( m_snippets );
	}
	m_snippets.Sort();
	m_modified = false;
	m_clipboard.Empty();
}
//------------------------------------------------------------
SnipWiz::~SnipWiz()
{
	if ( m_modified )
		m_StringDb.Save( m_pluginPath + defaultTmplFile );

	m_topWin->Disconnect( IDM_SETTINGS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SnipWiz::OnSettings ), NULL, this );
	m_topWin->Disconnect( IDM_CLASS_WIZ, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SnipWiz::OnClassWizard ), NULL, this );

	m_topWin->Disconnect( IDM_EXP_SWITCH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SnipWiz::OnMenuExpandSwitch ), NULL, this );
	m_topWin->Disconnect( IDM_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SnipWiz::OnMenuPaste ), NULL, this );

	DetachDynMenus();
}
//------------------------------------------------------------

wxToolBar *SnipWiz::CreateToolBar( wxWindow *parent )
{
	return NULL;
}
//------------------------------------------------------------

void SnipWiz::CreatePluginMenu( wxMenu *pluginsMenu )
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item( NULL );

	item = new wxMenuItem( menu, IDM_SETTINGS, wxT( "Settings..." ), wxT( "Settings..." ), wxITEM_NORMAL );
	menu->Append( item );
	item = new wxMenuItem( menu, IDM_CLASS_WIZ, wxT( "Template class..." ), wxT( "Template class..." ), wxITEM_NORMAL );
	menu->Append( item );

	pluginsMenu->Append( wxID_ANY, plugName, menu );


	m_topWin->Connect( IDM_SETTINGS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SnipWiz::OnSettings ), NULL, this );
	m_topWin->Connect( IDM_CLASS_WIZ, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SnipWiz::OnClassWizard ), NULL, this );

	m_topWin->Connect( IDM_EXP_SWITCH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SnipWiz::OnMenuExpandSwitch ), NULL, this );
	m_topWin->Connect( IDM_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SnipWiz::OnMenuPaste ), NULL, this );
	AttachDynMenus();
}
//------------------------------------------------------------
void SnipWiz::HookPopupMenu( wxMenu *menu, MenuType type )
{
	if ( type == MenuTypeEditor ) {
		if ( !m_sepItem ) {
			m_sepItem = menu->AppendSeparator();
		}

		if ( !menu->FindItem( IDM_BASE ) ) {
			wxMenu *newMenu = CreateSubMenu();
			menu->Append( IDM_BASE, plugName, newMenu );
		}
	} else 	if (type == MenuTypeFileView_Folder) {
		//Create the popup menu for the virtual folders
		wxMenuItem *item(NULL);

		item = new wxMenuItem(menu, wxID_SEPARATOR);
		menu->Prepend(item);
		m_vdDynItems.push_back(item);

		item = new wxMenuItem(menu, IDM_CLASS_WIZ, wxT("New Class from Template..."), wxEmptyString, wxITEM_NORMAL);
		menu->Prepend(item);
		m_vdDynItems.push_back(item);
	}
}

//------------------------------------------------------------

void SnipWiz::UnHookPopupMenu( wxMenu *menu, MenuType type )
{
	if ( type == MenuTypeEditor ) {
		wxMenuItem *item = menu->FindItem( IDM_BASE );
		if ( item ) {
			menu->Destroy( item );
		}

		if ( m_sepItem ) {
			menu->Destroy( m_sepItem );
			m_sepItem = NULL;
		}
	} else if (type == MenuTypeFileView_Folder) {
		std::vector<wxMenuItem*>::iterator iter = m_vdDynItems.begin();
		for (; iter != m_vdDynItems.end(); iter++) {
			menu->Destroy(*iter);
		}
		m_vdDynItems.clear();
	}
}

//------------------------------------------------------------

void SnipWiz::UnPlug()
{
	//TODO:: perform the unplug action for this plugin
}
//------------------------------------------------------------
void SnipWiz::OnMenuExpandSwitch( wxCommandEvent& e )
{
	IEditor *editor = GetEditor();
	if ( !editor )
		return;

	bool isSelection = false;
	wxString var = editor->GetSelection();
	if ( !var.IsEmpty() )
		isSelection = true;
	var = ::wxGetTextFromUser( wxT( "Enter identifier name" ), wxT( "switch(...)" ), var );
	if ( var.IsEmpty() )
		return;
	long count = ::wxGetNumberFromUser( wxT( "Enter number of cases" ), wxT( "Cases:" ), wxT( "switch(...)" ), 1, 1, 20 );
	if ( count < 1 )
		return;

	int curEol = editor->GetEOL();
	int curPos = editor->GetCurrentPosition();
	wxString tabs = GetTabs( editor, curPos );

	wxString output = wxString::Format( wxT( "switch( %s )%s%s{%s" ), var.c_str(), eol[curEol].c_str(), tabs.c_str(), eol[curEol].c_str() );
	for ( long i = 0;i < count;i++ )
		output += wxString::Format( wxT( "%scase :%s%sbreak;%s" ), tabs.c_str(), eol[curEol].c_str(), tabs.c_str(), eol[curEol].c_str() );

	output += tabs.c_str();
	output += wxT( "}" );
	if ( isSelection )
		editor->ReplaceSelection( output );
	else
		editor->InsertText( curPos, output );
}

//------------------------------------------------------------
void SnipWiz::OnMenuSnippets( wxCommandEvent &e )
{
	IEditor *editor = GetEditor();
	if ( !editor )
		return;

	bool crtl = ::wxGetKeyState( WXK_CONTROL );
	bool sourceIsMenu( false );
	
	wxMenu *m = dynamic_cast<wxMenu*>( e.GetEventObject() );
	if(m) {
		sourceIsMenu = true;
	}
		
	if ( e.GetId() >= IDM_ADDSTART && e.GetId() < ( IDM_ADDSTART + (int)m_snippets.GetCount() ) ) {
		wxString key = m_snippets.Item( e.GetId() - IDM_ADDSTART );
		wxString srText = m_StringDb.GetSnippetString( key );
		wxString selection = editor->GetSelection();
		// replace template eols with current
		int curEol = editor->GetEOL();
		if(srText.Find(eol[2])  != wxNOT_FOUND )
			srText.Replace( eol[2], eol[curEol].c_str() );
		// selection ?
		if ( srText.Find( SELECTION ) != wxNOT_FOUND )
			srText.Replace( SELECTION, selection.c_str() );

		// if the user pressed control while clicking
		if ( crtl && sourceIsMenu ) {
			m_clipboard = srText;
			// remove caret mark if there
			srText.Replace( CARET, wxT( "" ) );
			// copy text to clipboard
			if ( wxTheClipboard->Open() ) {
				wxTheClipboard->SetData( new wxTextDataObject( srText ) );
				wxTheClipboard->Close();
			}
		} else {
			// otherwise insert text
			wxString output = FormatOutput( editor, srText );
			int curPos = editor->GetCurrentPosition() - selection.Len();
			// get caret position
			long cursorPos = output.Find( CARET );
			if ( cursorPos != wxNOT_FOUND )
				output.Remove( cursorPos, 1 );
			editor->ReplaceSelection( output );
			// set caret
			if ( cursorPos != wxNOT_FOUND )
				editor->SetCaretAt( curPos + cursorPos );
			else
				editor->SetCaretAt( curPos + output.Len() );
		}
	}
}
//------------------------------------------------------------
void SnipWiz::OnMenuPaste( wxCommandEvent& e )
{
	wxUnusedVar( e );
	IEditor *editor = GetEditor();
	if ( !editor )
		return;

	if ( m_clipboard.IsEmpty() )
		return;
	// otherwise insert text
	wxString output = FormatOutput( editor, m_clipboard );
	wxString selection = editor->GetSelection();
	int curPos = editor->GetCurrentPosition() - selection.Len();
	// get caret position
	long cursorPos = output.Find( CARET );
	if ( cursorPos != wxNOT_FOUND )
		output.Remove( cursorPos, 1 );
	editor->ReplaceSelection( output );
	// set caret
	if ( cursorPos != wxNOT_FOUND )
		editor->SetCaretAt( curPos + cursorPos );
	else
		editor->SetCaretAt( curPos + output.Len() );
}
//------------------------------------------------------------
wxString SnipWiz::FormatOutput( IEditor* pEditor, const wxString& text )
{
	wxString output = text;
	int curPos = pEditor->GetCurrentPosition();
	int curEol = pEditor->GetEOL();
	wxString tabs = GetTabs( pEditor, curPos );
	output.Replace( eol[curEol], eol[curEol] + tabs );
	return output;
}

//------------------------------------------------------------
wxMenu* SnipWiz::CreateSubMenu()
{
	wxMenu *parentMenu = new wxMenu();

	wxMenuItem *item( NULL );
	if ( !m_clipboard.IsEmpty() ) {
		item = new wxMenuItem( parentMenu, IDM_PASTE, wxT( "Paste buffer" ), wxT( "Paste buffer" ), wxITEM_NORMAL );
		parentMenu->Append( item );
		parentMenu->AppendSeparator();
	}
	item = new wxMenuItem( parentMenu, IDM_EXP_SWITCH, wxT( "switch{...}" ), wxT( "switch{...}" ), wxITEM_NORMAL );
	parentMenu->Append( item );
	parentMenu->AppendSeparator();

	for ( wxUint32 i = 0;i < m_snippets.GetCount();i++ ) {
		item = new wxMenuItem( parentMenu, IDM_ADDSTART + i, m_snippets.Item( i ), m_snippets.Item( i ), wxITEM_NORMAL );
		parentMenu->Append( item );
	}

	return parentMenu;
}
//------------------------------------------------------------
// opens settings
void SnipWiz::OnSettings( wxCommandEvent& e )
{
	EditSnippetsDlg dlg( m_mgr->GetTheApp()->GetTopWindow(), this, m_mgr );
	dlg.ShowModal();

	if ( dlg.GetModified() ) {
		m_snippets.Clear();
		m_StringDb.GetAllSnippetKeys( m_snippets );
		m_snippets.Sort();
		DetachDynMenus();
		AttachDynMenus();
		m_modified = true;
	}
}

//------------------------------------------------------------
// generate some default entries
void SnipWiz::IntSnippets()
{
	m_StringDb.SetSnippetString( wxT( "//-" ), wxT( "//------------------------------------------------------------@" ) );
	m_StringDb.SetSnippetString( wxT( "wxT(\"" ), wxT( "wxT(\"@\")" ) );
	m_StringDb.SetSnippetString( wxT( "wxT($" ), wxT( "wxT( $ )" ) );
	m_StringDb.SetSnippetString( wxT( "if($" ) , wxT( "if ( $ )\n{\n\t@\n}" ) );
	m_StringDb.SetSnippetString( wxT( "whi($" ) , wxT( "while ( $ )\n{\n\t@\n}" ) );
	m_StringDb.SetSnippetString( wxT( "($)" ), wxT( "( $ )@" ) );
	m_StringDb.SetSnippetString( wxT( "{$}" ) , wxT( "{ $ }@" ) );
	m_StringDb.SetSnippetString( wxT( "[$]" ), wxT( "[ $ ]@" ) );
	m_StringDb.SetSnippetString( wxT( "wxU($" ), wxT( "wxUnusedVar( $ );@" ) );
	m_StringDb.SetSnippetString( wxT( "for($" ), wxT( "for( $ = 0; $  < @; $++ )" ) );
	m_StringDb.SetSnippetString( wxT( "for(Ii" ), wxT( "for( int i = 0; i  < $; i++ )@" ) );
	m_StringDb.SetSnippetString( wxT( "for(Ui" ), wxT( "for( unsigned int i = 0; i  < $; i++ )@" ) );
}

//------------------------------------------------------------
// detach dynamic menus
void SnipWiz::DetachDynMenus()
{
	m_topWin->Disconnect( IDM_ADDSTART, IDM_ADDSTART + m_snippets.GetCount() - 1, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SnipWiz::OnMenuSnippets ), NULL, this );
}
//------------------------------------------------------------
// attach dynamic menus
void SnipWiz::AttachDynMenus()
{
	m_topWin->Connect( IDM_ADDSTART, IDM_ADDSTART + m_snippets.GetCount() - 1, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SnipWiz::OnMenuSnippets ), NULL, this );
}
//------------------------------------------------------------
// find current indentation
long SnipWiz::GetCurrentIndentation( IEditor *pEditor, long pos )
{
	wxString buffer = pEditor->GetEditorText().Left( pos );
	long tabCount = 0;
	wxChar ch = ( pEditor->GetEOL() == 1 ) ? wxT( '\r' ) : wxT( '\n' );
	buffer = buffer.AfterLast( ch );
	for ( long i = 0;i < (long)buffer.Len();i++ )
		if ( buffer.GetChar( i ) == wxT( '\t' ) )
			tabCount++;

	return tabCount;
}
//------------------------------------------------------------
// generates a string with tabs for current indentation
wxString SnipWiz::GetTabs( IEditor* pEditor, long pos )
{
	long indent = GetCurrentIndentation( pEditor, pos );
	wxString tabs = wxT( "" );
	for ( long i = 0;i < indent;i++ )
		tabs += wxT( "\t" );
	return tabs;
}
//------------------------------------------------------------
// returns pointer to editor
IEditor* SnipWiz::GetEditor()
{
	IEditor *editor = m_mgr->GetActiveEditor();
	if ( !editor ) {
		::wxMessageBox( noEditor, codeLite, wxICON_WARNING | wxOK );
		return NULL;
	}
	return editor;
}
//------------------------------------------------------------
void SnipWiz::OnClassWizard( wxCommandEvent& e )
{
	TemplateClassDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), this, m_mgr);
	
	wxString errMsg, projectPath, projectName;
	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo( TreeFileView );

	projectName = m_mgr->GetWorkspace()->GetActiveProjectName();
	if ( m_mgr->GetWorkspace() ) {
		if ( item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory ) {
			projectPath =  item.m_fileName.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR );
		} else {
			ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName( projectName, errMsg );
			if ( proj )
				projectPath =  proj->GetFileName().GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR );
		}
	}

	dlg.SetCurEol(GetEOLByOS());
	dlg.SetPluginPath(m_pluginPath );
	dlg.SetProjectPath( projectPath );
	dlg.ShowModal();
	
	if ( dlg.GetModified() ){
		m_modified = true;
	}
	
}
//------------------------------------------------------------
