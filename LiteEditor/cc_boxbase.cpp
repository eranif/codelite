///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cc_boxbase.h"
#include <wx/pen.h>
#if wxVERSION_NUMBER >= 2809
#include <wx/aui/auibar.h>
#else
	#include <wx/toolbar.h>
#endif
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
	wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer* topSizer  = new wxBoxSizer( wxHORIZONTAL );
	SetSizer(mainSizer);

	m_mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE|wxTAB_TRAVERSAL);
#ifdef __WXGTK__
	m_mainPanel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
#endif
	m_mainPanel->SetSizer(topSizer);

	mainSizer->Add(m_mainPanel, 1, wxEXPAND);

	//this->SetBackgroundColour(*wxBLACK);
	m_listCtrl = new CCVirtualListCtrl( m_mainPanel, wxID_ANY, wxDefaultPosition, wxSize(BOX_WIDTH - 30, BOX_HEIGHT), wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VIRTUAL|wxBORDER_STATIC );
#ifdef __WXGTK__
	m_listCtrl->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
	m_listCtrl->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
#endif

	wxBitmap bmp         = PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("cc/16/lock"));
	wxBitmap commentsBmp = PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("cc/16/note"));

#if wxVERSION_NUMBER >= 2809
	m_toolBar1 = new clToolBar( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, (1 << 5) /* wxAUI_TB_VERTICAL */);
#else
	m_toolBar1 = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_VERTICAL);
#endif
	m_toolBar1->AddTool( TOOL_SHOW_PRIVATE_MEMBERS,
						 _("Show Protected / Private Items"),
						 bmp,
						 bmp,
						 wxITEM_CHECK,
						 _("If pressed, will show 'protected' and 'private' items as well as 'public' ones"),
						 wxEmptyString,
						 NULL);

	m_toolBar1->AddTool( TOOL_SHOW_ITEM_COMMENTS,
						 _("Show Item Comments"),
						 commentsBmp,
						 commentsBmp,
						 wxITEM_CHECK,
						 _("Show Item Comments"),
						 wxEmptyString,
						 NULL);

	m_toolBar1->Realize();

#ifdef __WXGTK__
	topSizer->Add( m_listCtrl, 1, wxEXPAND );
	topSizer->Add( m_toolBar1, 0, wxEXPAND );
#else
	topSizer->Add( m_listCtrl, 1, wxEXPAND|wxLEFT|wxTOP|wxBOTTOM, 2 );
	topSizer->Add( m_toolBar1, 0, wxEXPAND|wxRIGHT|wxTOP|wxBOTTOM, 2 );
#endif

    m_mainPanel->SetAutoLayout( true );
    GetSizer()->Fit(this);

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
