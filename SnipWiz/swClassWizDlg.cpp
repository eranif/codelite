/////////////////////////////////////////////////////////////////////////////
// Name:        swclasswizdlg.cpp
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     02/09/2008 09:34:22
// RCS-ID:
// Copyright:   2008 Frank Lichtner
// Licence:		GNU General Public Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

//#include <frltrace.h>
////@begin includes
////@end includes
#include <wx/textfile.h>
#include "plugin.h"
#include "workspace.h"
#include "virtualdirectoryselector.h"
#include "snipwiz.h"

#include "swGlobals.h"
#include "swclasswizdlg.h"
////@begin XPM images
////@end XPM images

static const wxString swHeader = wxT( "header" );
static const wxString swSource = wxT( "source" );
static const wxString swPhClass = wxT( "%CLASS%" );

//------------------------------------------------------------
// swClassWizDlg type definition
IMPLEMENT_DYNAMIC_CLASS( swClassWizDlg, wxDialog )

//------------------------------------------------------------
// swClassWizDlg event table definition
BEGIN_EVENT_TABLE( swClassWizDlg, wxDialog )

	////@begin swClassWizDlg event table entries
    EVT_INIT_DIALOG( swClassWizDlg::OnInitDialog )

    EVT_TEXT( ID_TEXTCTRL, swClassWizDlg::OnClassNameUpdated )

    EVT_BUTTON( ID_GET_VIRT_FOLDER, swClassWizDlg::OnGetVirtFolderClick )

    EVT_BUTTON( ID_GET_HD_FOLDER, swClassWizDlg::OnGetHdFolderClick )

    EVT_BUTTON( ID_GENERATE, swClassWizDlg::OnGenerateClick )
    EVT_UPDATE_UI( ID_GENERATE, swClassWizDlg::OnGenerateUpdate )

    EVT_COMBOBOX( ID_TEMPLATES, swClassWizDlg::OnTemplatesSelected )

    EVT_BUTTON( ID_ADD_TEMPLATE, swClassWizDlg::OnAddTemplateClick )
    EVT_UPDATE_UI( ID_ADD_TEMPLATE, swClassWizDlg::OnAddTemplateUpdate )

    EVT_BUTTON( ID_CHANGE_TEMPLATE, swClassWizDlg::OnChangeTemplateClick )
    EVT_UPDATE_UI( ID_CHANGE_TEMPLATE, swClassWizDlg::OnChangeTemplateUpdate )

    EVT_BUTTON( ID_REMOVE_TEMPLATE, swClassWizDlg::OnRemoveTemplateClick )
    EVT_UPDATE_UI( ID_REMOVE_TEMPLATE, swClassWizDlg::OnRemoveTemplateUpdate )

    EVT_BUTTON( ID_CLEAR, swClassWizDlg::OnClearClick )

    EVT_BUTTON( ID_CLASS_VAR, swClassWizDlg::OnClassVarClick )

	////@end swClassWizDlg event table entries

END_EVENT_TABLE()

//------------------------------------------------------------
// swClassWizDlg constructors
swClassWizDlg::swClassWizDlg()
{
	Init();
}
//------------------------------------------------------------
swClassWizDlg::swClassWizDlg( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create( parent, id, caption, pos, size, style );
}

//------------------------------------------------------------
// swClassWizDlg creator
bool swClassWizDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin swClassWizDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end swClassWizDlg creation
	return true;
}

//------------------------------------------------------------
// swClassWizDlg destructor
swClassWizDlg::~swClassWizDlg()
{
////@begin swClassWizDlg destruction
////@end swClassWizDlg destruction
	GetStringDb()->Save( m_pluginPath + defaultTmplFile );
}

//------------------------------------------------------------
// Member initialisation
void swClassWizDlg::Init()
{
////@begin swClassWizDlg member initialisation
    m_pluginPath = wxT("");
    m_curEol = 0;
    m_virtualFolder = wxT("");
    m_projectPath = wxT("");
    m_pClassName = NULL;
    m_pCurrentTemplate = NULL;
    m_pH_filename = NULL;
    m_pCPP_filename = NULL;
    m_pVFolderText = NULL;
    m_pVirtFolder = NULL;
    m_pHdPath = NULL;
    m_pGenerate = NULL;
    m_pTemplates = NULL;
    m_pFilesNotebook = NULL;
    m_pHeaderText = NULL;
    m_pSourceText = NULL;
////@end swClassWizDlg member initialisation
	m_modified = false;
	m_pPlugin = NULL;
}

//------------------------------------------------------------
// Control creation for swClassWizDlg
void swClassWizDlg::CreateControls()
{
////@begin swClassWizDlg content construction
    swClassWizDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxNotebook* itemNotebook3 = new wxNotebook( itemDialog1, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );

    wxPanel* itemPanel4 = new wxPanel( itemNotebook3, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemPanel4->SetSizer(itemBoxSizer5);

    itemBoxSizer5->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    wxStaticBox* itemStaticBoxSizer7Static = new wxStaticBox(itemPanel4, wxID_ANY, _("Class"));
    wxStaticBoxSizer* itemStaticBoxSizer7 = new wxStaticBoxSizer(itemStaticBoxSizer7Static, wxVERTICAL);
    itemBoxSizer5->Add(itemStaticBoxSizer7, 0, wxGROW|wxLEFT|wxRIGHT, 40);
    wxFlexGridSizer* itemFlexGridSizer8 = new wxFlexGridSizer(3, 2, 0, 0);
    itemFlexGridSizer8->AddGrowableCol(1);
    itemStaticBoxSizer7->Add(itemFlexGridSizer8, 0, wxGROW|wxALL, 5);
    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel4, wxID_STATIC, _("Class name:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText9, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_pClassName = new wxTextCtrl( itemPanel4, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize(200, -1), 0 );
    if (swClassWizDlg::ShowToolTips())
        m_pClassName->SetToolTip(_("Name of new class"));
    itemFlexGridSizer8->Add(m_pClassName, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel4, wxID_STATIC, _("Template:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(itemStaticText11, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_pCurrentTemplateStrings;
    m_pCurrentTemplate = new wxComboBox( itemPanel4, ID_CURRENT_TEMPLATE, _T(""), wxDefaultPosition, wxSize(200, -1), m_pCurrentTemplateStrings, wxCB_READONLY|wxCB_SORT );
    if (swClassWizDlg::ShowToolTips())
        m_pCurrentTemplate->SetToolTip(_("Template for new class"));
    itemFlexGridSizer8->Add(m_pCurrentTemplate, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    itemBoxSizer5->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    wxStaticBox* itemStaticBoxSizer14Static = new wxStaticBox(itemPanel4, wxID_ANY, _("Files"));
    wxStaticBoxSizer* itemStaticBoxSizer14 = new wxStaticBoxSizer(itemStaticBoxSizer14Static, wxVERTICAL);
    itemBoxSizer5->Add(itemStaticBoxSizer14, 0, wxGROW|wxLEFT|wxRIGHT, 40);
    wxFlexGridSizer* itemFlexGridSizer15 = new wxFlexGridSizer(4, 3, 0, 0);
    itemFlexGridSizer15->AddGrowableCol(1);
    itemStaticBoxSizer14->Add(itemFlexGridSizer15, 0, wxGROW|wxALL, 5);
    wxStaticText* itemStaticText16 = new wxStaticText( itemPanel4, wxID_STATIC, _(".h filename:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(itemStaticText16, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_pH_filename = new wxTextCtrl( itemPanel4, ID_TEXTCTRL1, _T(""), wxDefaultPosition, wxSize(200, -1), 0 );
    if (swClassWizDlg::ShowToolTips())
        m_pH_filename->SetToolTip(_("Name of header file"));
    itemFlexGridSizer15->Add(m_pH_filename, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    itemFlexGridSizer15->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText19 = new wxStaticText( itemPanel4, wxID_STATIC, _(".cpp filename:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(itemStaticText19, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_pCPP_filename = new wxTextCtrl( itemPanel4, ID_TEXTCTRL2, _T(""), wxDefaultPosition, wxSize(200, -1), 0 );
    if (swClassWizDlg::ShowToolTips())
        m_pCPP_filename->SetToolTip(_("Name of source file"));
    itemFlexGridSizer15->Add(m_pCPP_filename, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    itemFlexGridSizer15->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_pVFolderText = new wxStaticText( itemPanel4, ID_VIRTFOLDER_TEXT, _("Project tree folder:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pVFolderText->SetForegroundColour(wxColour(215, 0, 0));
    itemFlexGridSizer15->Add(m_pVFolderText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_pVirtFolder = new wxTextCtrl( itemPanel4, ID_VIRTFOLDER, _T(""), wxDefaultPosition, wxSize(230, -1), wxTE_READONLY );
    if (swClassWizDlg::ShowToolTips())
        m_pVirtFolder->SetToolTip(_("Virtual folder to add new files"));
    m_pVirtFolder->SetBackgroundColour(wxColour(255, 255, 230));
    m_pVirtFolder->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, wxT("Tahoma")));
    itemFlexGridSizer15->Add(m_pVirtFolder, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    wxButton* itemButton24 = new wxButton( itemPanel4, ID_GET_VIRT_FOLDER, _("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    if (swClassWizDlg::ShowToolTips())
        itemButton24->SetToolTip(_("Browse virtual folders"));
    itemFlexGridSizer15->Add(itemButton24, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    wxStaticText* itemStaticText25 = new wxStaticText( itemPanel4, wxID_STATIC, _("Path:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(itemStaticText25, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_pHdPath = new wxTextCtrl( itemPanel4, ID_TEXTCTRL3, _T(""), wxDefaultPosition, wxSize(230, -1), wxTE_READONLY );
    if (swClassWizDlg::ShowToolTips())
        m_pHdPath->SetToolTip(_("Hd folder to add new files"));
    m_pHdPath->SetBackgroundColour(wxColour(255, 255, 230));
    itemFlexGridSizer15->Add(m_pHdPath, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    wxButton* itemButton27 = new wxButton( itemPanel4, ID_GET_HD_FOLDER, _("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    if (swClassWizDlg::ShowToolTips())
        itemButton27->SetToolTip(_("Browse  folders"));
    itemFlexGridSizer15->Add(itemButton27, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer28, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    m_pGenerate = new wxButton( itemPanel4, ID_GENERATE, _("&Generate"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pGenerate->SetDefault();
    m_pGenerate->Enable(false);
    itemBoxSizer28->Add(m_pGenerate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton30 = new wxButton( itemPanel4, wxID_CANCEL, _("&Quit"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(itemButton30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemNotebook3->AddPage(itemPanel4, _("Generate"));

    wxPanel* itemPanel31 = new wxPanel( itemNotebook3, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer32 = new wxBoxSizer(wxVERTICAL);
    itemPanel31->SetSizer(itemBoxSizer32);

    wxBoxSizer* itemBoxSizer33 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer32->Add(itemBoxSizer33, 0, wxGROW|wxALL, 1);
    wxStaticText* itemStaticText34 = new wxStaticText( itemPanel31, wxID_STATIC, _("Class:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer33->Add(itemStaticText34, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxArrayString m_pTemplatesStrings;
    m_pTemplates = new wxComboBox( itemPanel31, ID_TEMPLATES, _T(""), wxDefaultPosition, wxDefaultSize, m_pTemplatesStrings, wxCB_SORT );
    itemBoxSizer33->Add(m_pTemplates, 1, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxButton* itemButton36 = new wxButton( itemPanel31, ID_ADD_TEMPLATE, _("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    itemButton36->SetDefault();
    itemBoxSizer33->Add(itemButton36, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxButton* itemButton37 = new wxButton( itemPanel31, ID_CHANGE_TEMPLATE, _("Change"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    itemBoxSizer33->Add(itemButton37, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxButton* itemButton38 = new wxButton( itemPanel31, ID_REMOVE_TEMPLATE, _("Remove"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    itemBoxSizer33->Add(itemButton38, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxButton* itemButton39 = new wxButton( itemPanel31, ID_CLEAR, _("Clear"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    itemButton39->SetDefault();
    itemBoxSizer33->Add(itemButton39, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    itemBoxSizer33->Add(5, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton41 = new wxButton( itemPanel31, ID_CLASS_VAR, _("%CLASS%"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
    itemBoxSizer33->Add(itemButton41, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_pFilesNotebook = new wxNotebook( itemPanel31, ID_FILES_NOTEBOOK, wxDefaultPosition, wxSize(500, -1), wxBK_DEFAULT|wxBK_TOP );

    m_pHeaderText = new wxTextCtrl( m_pFilesNotebook, ID_TEXT_HEADER, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_TAB|wxTE_RICH|wxTE_RICH2|wxTE_NOHIDESEL );
    m_pHeaderText->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Courier New")));

    m_pFilesNotebook->AddPage(m_pHeaderText, _("Header file"));

    m_pSourceText = new wxTextCtrl( m_pFilesNotebook, ID_TEXT_SOURCE, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_TAB|wxTE_RICH|wxTE_NOHIDESEL );
    m_pSourceText->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Courier New")));

    m_pFilesNotebook->AddPage(m_pSourceText, _("Implementation file"));

    itemBoxSizer32->Add(m_pFilesNotebook, 1, wxGROW|wxALL, 1);

    itemNotebook3->AddPage(itemPanel31, _("Templates"));

    itemBoxSizer2->Add(itemNotebook3, 1, wxGROW|wxALL, 2);

////@end swClassWizDlg content construction
}

//------------------------------------------------------------
// Should we show tooltips?
bool swClassWizDlg::ShowToolTips()
{
	return true;
}

//------------------------------------------------------------
// template page
//------------------------------------------------------------
// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_TEMPLATES
void swClassWizDlg::OnTemplatesSelected( wxCommandEvent& event )
{
	wxUnusedVar( event );
	wxString set = m_pTemplates->GetValue();
	if ( GetStringDb()->IsSet( set ) )
	{
		m_pHeaderText->SetValue( GetStringDb()->GetString( set, swHeader ) );
		m_pSourceText->SetValue( GetStringDb()->GetString( set, swSource ) );
	}
}

//------------------------------------------------------------
// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ADD_TEMPLATE
void swClassWizDlg::OnAddTemplateClick( wxCommandEvent& event )
{
	wxUnusedVar( event );
	wxString set = m_pTemplates->GetValue();
	bool isSet = GetStringDb()->IsSet( set );
	if ( isSet )
	{
		int ret = ::wxMessageBox( wxT( "Class exists!\nOverwrite?" ), wxT( "Add class" ), wxYES_NO | wxICON_QUESTION );
		if ( ret == wxNO )
			return;
	}
	GetStringDb()->SetString( set, swHeader, m_pHeaderText->GetValue() );
	GetStringDb()->SetString( set, swSource, m_pSourceText->GetValue() );
	if ( !isSet )
		m_pTemplates->AppendString( set );
	RefreshTemplateList();
	m_modified = true;
}
//------------------------------------------------------------
// wxEVT_UPDATE_UI event handler for ID_ADD_TEMPLATE
void swClassWizDlg::OnAddTemplateUpdate( wxUpdateUIEvent& event )
{
	if ( m_pTemplates->GetValue().IsEmpty() || m_pHeaderText->GetValue().IsEmpty() || m_pSourceText->GetValue().IsEmpty() )
		event.Enable( false );
	else
		event.Enable( true );
}
//------------------------------------------------------------
// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CHANGE_TEMPLATE
void swClassWizDlg::OnChangeTemplateClick( wxCommandEvent& event )
{
	wxUnusedVar( event );
	wxString set = m_pTemplates->GetValue();
	bool isSet = GetStringDb()->IsSet( set );
	if ( !isSet )
	{
		int ret = ::wxMessageBox( wxT( "Class doesn't exists!\nAdd new?" ), wxT( "Change class" ), wxYES_NO | wxICON_QUESTION );
		if ( ret == wxNO )
			return;
	}
	GetStringDb()->SetString( set, swHeader, m_pHeaderText->GetValue() );
	GetStringDb()->SetString( set, swSource, m_pSourceText->GetValue() );
	if ( !isSet )
		m_pTemplates->AppendString( set );
	RefreshTemplateList();
	m_modified = true;
}

//------------------------------------------------------------
// wxEVT_UPDATE_UI event handler for ID_CHANGE_TEMPLATE
void swClassWizDlg::OnChangeTemplateUpdate( wxUpdateUIEvent& event )
{
	if ( m_pTemplates->GetSelection() == wxNOT_FOUND )
		event.Enable( false );
	else
		event.Enable( true );
	if ( m_pHeaderText->IsModified() == false && m_pSourceText->IsModified() == false )
		event.Enable( false );
}

//------------------------------------------------------------
// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_REMOVE_TEMPLATE
void swClassWizDlg::OnRemoveTemplateClick( wxCommandEvent& event )
{
	wxUnusedVar( event );
	wxString set = m_pTemplates->GetValue();
	bool isSet = GetStringDb()->IsSet( set );
	if ( !isSet )
	{
		::wxMessageBox( wxT( "Class not found!\nNothing to remove." ), wxT( "Remove class" ) );
		return;
	}
	GetStringDb()->DeleteKey( set, swHeader );
	GetStringDb()->DeleteKey( set, swSource );
	long index = m_pTemplates->FindString( set );
	m_pTemplates->Delete( index );
	RefreshTemplateList();
	m_modified = true;
}

//------------------------------------------------------------
// wxEVT_UPDATE_UI event handler for ID_REMOVE_TEMPLATE
void swClassWizDlg::OnRemoveTemplateUpdate( wxUpdateUIEvent& event )
{
	if ( m_pTemplates->GetSelection() == wxNOT_FOUND )
		event.Enable( false );
	else
		event.Enable( true );
}

//------------------------------------------------------------
// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CLASS_VAR
void swClassWizDlg::OnClassVarClick( wxCommandEvent& event )
{
	wxUnusedVar( event );
	long from, to;

	if ( m_pFilesNotebook->GetSelection() == 0 )
	{
		m_pHeaderText->GetSelection( &from, &to );
		m_pHeaderText->Replace( from, to, swPhClass );
		m_pHeaderText->SetFocus();
	}
	else
	{
		m_pSourceText->GetSelection( &from, &to );
		m_pSourceText->Replace( from, to, swPhClass );
		m_pSourceText->SetFocus();
	}
}

//------------------------------------------------------------
// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_COMMENT_LINE
void swClassWizDlg::OnClearClick( wxCommandEvent& event )
{
	m_pTemplates->SetValue( wxT( "" ) );
	m_pSourceText->SetValue( wxT( "" ) );
	m_pHeaderText->SetValue( wxT( "" ) );
}
//------------------------------------------------------------
// generate page
//------------------------------------------------------------
// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL
void swClassWizDlg::OnClassNameUpdated( wxCommandEvent& event )
{
	wxUnusedVar( event );
	wxString buffer = m_pClassName->GetValue();
	if ( buffer.IsEmpty() )
	{
		m_pH_filename->SetValue(wxT(""));
		m_pCPP_filename->SetValue(wxT(""));
		return;
	}
	else
	{
		m_pH_filename->SetValue( buffer.Lower() + wxT( ".h" ) );
		m_pCPP_filename->SetValue( buffer.Lower() + wxT( ".cpp" ) );
	}
}

//------------------------------------------------------------
// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_GENERATE
void swClassWizDlg::OnGenerateClick( wxCommandEvent& event )
{
	wxArrayString files;
	wxString newClassName = m_pClassName->GetValue();
	wxString baseClass = m_pCurrentTemplate->GetValue();
	
	if(!wxEndsWithPathSeparator(m_projectPath))
		m_projectPath += wxFILE_SEP_PATH;

	wxString buffer = GetStringDb()->GetString( baseClass, swHeader );
	buffer.Replace( swPhClass, newClassName );
	files.Add( m_projectPath + m_pH_filename->GetValue() );
	SaveBufferToFile( files[0], buffer );

	buffer = wxString::Format( wxT( "#include \"%s\"%s%s" ), m_pH_filename->GetValue().c_str(), eol[m_curEol].c_str(), eol[m_curEol].c_str() );
	buffer += GetStringDb()->GetString( baseClass, swSource );
	buffer.Replace( swPhClass, newClassName );
	files.Add( m_projectPath + m_pCPP_filename->GetValue() );
	SaveBufferToFile( files[1], buffer );
	
	if ( !m_pVirtFolder->GetValue().IsEmpty() )
	{
		m_pManager->AddFilesToVirtualFodler( m_pVirtFolder->GetValue(), files );
	}
	wxString msg;
	msg << wxString::Format( wxT( "%s\n" ), files[0].c_str() )
	<< wxString::Format( wxT( "%s\n\n" ), files[1].c_str() )
	<< wxT( "Files successfully created." );
	::wxMessageBox( msg, wxT( "Add template class" ) );
	EndModal( wxID_OK );
}
	
//------------------------------------------------------------
// wxEVT_UPDATE_UI event handler for ID_GENERATE
void swClassWizDlg::OnGenerateUpdate( wxUpdateUIEvent& event )
{
	wxUnusedVar( event );
	if ( m_pCurrentTemplate->GetSelection() == wxNOT_FOUND ||
	        m_pClassName->GetValue().IsEmpty() ||
	        m_pH_filename->GetValue().IsEmpty() ||
	        m_pCPP_filename->GetValue().IsEmpty() )
		event.Enable( false );
	else
		event.Enable( true );
}

//------------------------------------------------------------
// wxEVT_INIT_DIALOG event handler for ID_SWCODEWIZDLG
void swClassWizDlg::OnInitDialog( wxInitDialogEvent& event )
{
	event.Skip();
	// set tab sizes
	wxTextAttr attribs = m_pHeaderText->GetDefaultStyle();
	wxArrayInt	tabs = attribs.GetTabs();
	int tab = 70;
	for ( int i = 1; i < 20; i++ )
		tabs.Add( tab * i );
	attribs.SetTabs( tabs );
	m_pHeaderText->SetDefaultStyle( attribs );
	m_pSourceText->SetDefaultStyle( attribs );

	GetStringDb()->Load( m_pluginPath + defaultTmplFile );

	wxArrayString templates;
	GetStringDb()->GetAllSets( templates );
	for ( wxUint32 i = 0; i < templates.GetCount(); i++ )
	{
		m_pTemplates->AppendString( templates[i] );
		m_pCurrentTemplate->AppendString( templates[i] );
	}
	if ( templates.GetCount() )
	{
		m_pTemplates->Select( 0 );
		wxString set = m_pTemplates->GetValue();
		m_pHeaderText->SetValue( GetStringDb()->GetString( set, swHeader ) );
		m_pSourceText->SetValue( GetStringDb()->GetString( set, swSource ) );
		m_pCurrentTemplate->Select( 0 );
	}
	TreeItemInfo item = m_pManager->GetSelectedTreeItemInfo( TreeFileView );
	if ( item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory )
	{
		m_virtualFolder = VirtualDirectorySelector::DoGetPath( m_pManager->GetTree( TreeFileView ), item.m_item, false );
		m_projectPath =  item.m_fileName.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR );
	}
	m_pVirtFolder->SetValue( m_virtualFolder );
	m_pHdPath->SetValue( m_projectPath );
}
//------------------------------------------------------------
bool swClassWizDlg::SaveBufferToFile( const wxString filename, const wxString buffer, int eolType )
{
	wxTextFile file( filename );
	wxTextFileType tft = wxTextFileType_Dos;
	if ( file.Exists() )
	{
		int ret = ::wxMessageBox( _( "File already exists!\n\n Overwrite?" ), _( "Generate class files" ), wxYES_NO | wxYES_DEFAULT | wxICON_EXCLAMATION );
		if ( ret == wxID_NO )
			return false;
	}
	switch ( m_curEol )
	{
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
//------------------------------------------------------------
void swClassWizDlg::RefreshTemplateList()
{
	wxArrayString templates;
	GetStringDb()->GetAllSets( templates );
	m_pCurrentTemplate->Clear();
	for ( wxUint32 i = 0; i < templates.GetCount(); i++ )
		m_pCurrentTemplate->AppendString( templates[i] );
	if ( templates.GetCount() )
		m_pCurrentTemplate->Select( 0 );
}

//------------------------------------------------------------
//  wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_GET_VIRT_FOLDER
void swClassWizDlg::OnGetVirtFolderClick( wxCommandEvent& event )
{
	wxUnusedVar( event );
	VirtualDirectorySelector dlg( this, m_pManager->GetWorkspace(), m_pVirtFolder->GetValue() );
	if ( dlg.ShowModal() == wxID_OK )
	{
		m_pVirtFolder->SetValue( dlg.GetVirtualDirectoryPath() );
		m_pVFolderText->SetForegroundColour(wxColour(0,128,0));
		m_pVFolderText->Refresh();
	}
}

//------------------------------------------------------------
//  wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_GET_HD_FOLDER
void swClassWizDlg::OnGetHdFolderClick( wxCommandEvent& event )
{
	wxUnusedVar( event );
	wxString dir = wxT("");
	if ( wxFileName::DirExists( m_projectPath ) )
	{
		dir = m_projectPath;
	}
	dir = wxDirSelector( wxT( "Select output folder" ), dir);
	if ( !dir.IsEmpty() )
	{
		m_projectPath = dir;
		m_pHdPath->SetValue(m_projectPath);
	}
}

