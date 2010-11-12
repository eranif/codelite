///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cc_boxbase.h"
#include <wx/pen.h>
#include <wx/aui/auibar.h>
#include <wx/xrc/xmlres.h>


#include "pluginmanager.h"
///////////////////////////////////////////////////////////////////////////

CCBoxBase::CCBoxBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
#if CC_USES_POPUPWIN
	: CCBoxParent( parent, CC_STYLE )
#else
	: wxPanel( parent, id, pos, size, style )
#endif
{
	SetSizeHints(BOX_WIDTH, BOX_HEIGHT);
	wxBoxSizer* topSizer = new wxBoxSizer( wxHORIZONTAL );
	m_mainPanel = new wxPanel(this);

	m_listCtrl = new CCVirtualListCtrl( m_mainPanel, wxID_ANY, wxDefaultPosition, wxSize(BOX_WIDTH - 30, BOX_HEIGHT), wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VIRTUAL|wxBORDER_NONE );

	wxBitmap bmp         = PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("cc/16/lock"));
	wxBitmap commentsBmp = PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("cc/16/note"));
	m_toolBar1 = new clToolBar( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, (1 << 5) /* wxAUI_TB_VERTICAL */);
	m_toolBar1->AddTool( TOOL_SHOW_PRIVATE_MEMBERS, 
						 wxT("Show Protected / Private Items"), 
						 bmp, 
						 bmp, 
						 wxITEM_CHECK, 
						 wxT("Show Only Public Items"), 
						 wxEmptyString,
						 NULL);
						 
	m_toolBar1->AddTool( TOOL_SHOW_ITEM_COMMENTS, 
						 wxT("Show Item Comments"), 
						 commentsBmp, 
						 commentsBmp, 
						 wxITEM_CHECK, 
						 wxT("Show Item Comments"), 
						 wxEmptyString,
						 NULL);
						 
	m_toolBar1->Realize();

	topSizer->Add( m_listCtrl, 1, wxEXPAND, 2 );
	topSizer->Add( m_toolBar1, 0, wxEXPAND, 2 );

    m_mainPanel->SetAutoLayout( true );
    m_mainPanel->SetSizer( topSizer );
    topSizer->Fit(m_mainPanel);
    topSizer->Fit(this);

	// Connect Events
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CCBoxBase::OnItemActivated ), NULL, this );
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CCBoxBase::OnItemDeSelected ), NULL, this );
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CCBoxBase::OnItemSelected ), NULL, this );
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler( CCBoxBase::OnKeyDown ), NULL, this );
	this->Connect( TOOL_SHOW_PRIVATE_MEMBERS, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( CCBoxBase::OnShowPublicItems ) );
	this->Connect( TOOL_SHOW_ITEM_COMMENTS, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( CCBoxBase::OnShowComments ) );
}

CCBoxBase::~CCBoxBase()
{
	// Disconnect Events
	m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CCBoxBase::OnItemActivated ), NULL, this );
	m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CCBoxBase::OnItemDeSelected ), NULL, this );
	m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CCBoxBase::OnItemSelected ), NULL, this );
	m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler( CCBoxBase::OnKeyDown ), NULL, this );
	this->Disconnect( TOOL_SHOW_PRIVATE_MEMBERS, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( CCBoxBase::OnShowPublicItems ) );
	this->Disconnect( TOOL_SHOW_ITEM_COMMENTS, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( CCBoxBase::OnShowComments ) );
}
