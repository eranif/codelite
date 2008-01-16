///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "open_type_dlg.h"
#include "ctags_manager.h"
#include <wx/xrc/xmlres.h>
#include "macros.h"
#include "wx/imaglist.h"

static int OpenTypeDlgTimerId = wxNewId();

BEGIN_EVENT_TABLE(OpenTypeDlg, wxDialog)
EVT_TIMER(OpenTypeDlgTimerId, OpenTypeDlg::OnTimer)
EVT_CHAR_HOOK(OpenTypeDlg::OnCharHook)
EVT_LIST_ITEM_ACTIVATED(wxID_ANY, OpenTypeDlg::OnItemActivated)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////

OpenTypeDlg::OpenTypeDlg( wxWindow* parent, TagsManager *tagsMgr, int id, wxString title, wxPoint pos, wxSize size, int style ) 
: wxDialog( parent, id, title, pos, size, style )
{
	m_tagsManager = tagsMgr;
	m_timer = new wxTimer(this, OpenTypeDlgTimerId);
	m_timer->Start(100);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	m_staticText = new wxStaticText( this, wxID_ANY, wxT("Select a type to open (you may use wildcards):"), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_staticText, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );

	m_textTypeName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_textTypeName, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );

	m_listTypes = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER|wxLC_REPORT);
	mainSizer->Add( m_listTypes, 1, wxALL|wxEXPAND, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_buttonOK, 0, wxALL, 5 );

	m_button2 = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_button2, 0, wxALL, 5 );

	mainSizer->Add( btnSizer, 0, wxALIGN_RIGHT, 5 );
	//assign image list to the dialog
	m_il = new wxImageList(16, 16, true);
	m_il->Add(wxXmlResource::Get()->LoadBitmap(_T("namespace")));
	m_il->Add(wxXmlResource::Get()->LoadBitmap(_T("class")));
    m_il->Add(wxXmlResource::Get()->LoadBitmap(_T("struct")));
	wxBitmap bmp;

	// typedef
	bmp = wxXmlResource::Get()->LoadBitmap(_T("typedef"));
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	m_il->Add(bmp);

	bmp = wxXmlResource::Get()->LoadBitmap(_T("enum"));
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	m_il->Add(bmp);

	Init();
	ConnectButton(m_buttonOK, OpenTypeDlg::OnOK);

	this->SetSizer( mainSizer );
	this->Layout();
}

OpenTypeDlg::~OpenTypeDlg()
{
	delete m_timer;
	delete m_il;
	m_tags.clear();
}

void OpenTypeDlg::Init()
{
	m_listTypes->InsertColumn(0, wxT("Symbol"));
	m_tagsManager->OpenType(m_tags);
	this->m_textTypeName->SetFocus();
}

void OpenTypeDlg::OnTimer(wxTimerEvent &event)
{
	wxUnusedVar(event);
	PopulateList();
}

void OpenTypeDlg::PopulateList()
{
	wxString filter = m_textTypeName->GetValue();
	if(filter.Trim().IsEmpty())
		return;
	
	//if filter is the same, dont update the view
	if(m_filter == filter)
		return;
	m_filter = filter;
	
	m_listTypes->Freeze();
	m_listTypes->DeleteAllItems();
	m_listTypes->SetImageList(m_il, wxIMAGE_LIST_SMALL);
	
	m_itemsData.clear();

	int row=0;
	//populate the list view
	for(size_t i=0; i<m_tags.size(); i++)
	{
		filter = filter.MakeLower();
		if(!filter.EndsWith(wxT("*"))){
			filter << wxT("*");
		}
		wxString tagName = m_tags.at(i)->GetName();
		tagName = tagName.MakeLower();
		if(wxMatchWild(filter, tagName)){
			// Set the item display name
			wxString name = m_tags.at(i)->GetName();
			long tmp = m_listTypes->InsertItem(row, name, GetTagImage(m_tags.at(i)->GetKind()));
			m_listTypes->SetItemData(tmp, (int)i);
			row++;
			//m_itemsData[m_tags.at(i)->Key()] = m_tags.at(i);
		}
	}

	m_listTypes->SetColumnWidth(0, wxLIST_AUTOSIZE);
	if(m_listTypes->GetItemCount() > 0){
		m_listTypes->Focus(0);
		m_listTypes->Select(0);
	}
	m_listTypes->Thaw();
}

int OpenTypeDlg::GetTagImage(const wxString &kind)
{
	if(kind == wxT("namespace"))	return 0;
	if(kind == wxT("class"))		return 1;
	if(kind == wxT("struct"))		return 2;
	if(kind == wxT("typedef"))		return 3;
	if(kind == wxT("enum"))			return 4;
	if(kind == wxT("union"))		return 2;
	return 1;
}

void OpenTypeDlg::OnCharHook(wxKeyEvent &event)
{
	if(event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER)
	{
		TryOpenAndEndModal();
		return;
	}
	event.Skip(); 
}

void OpenTypeDlg::TryOpenAndEndModal()
{
	long item = m_listTypes->GetFirstSelected();
	long data  = m_listTypes->GetItemData(item);
	if((int)m_tags.size() > data && data >= 0)
	{
		m_tag = m_tags.at(data);
		m_tags.clear();
		EndModal(wxID_OK);
	}
}

void OpenTypeDlg::OnOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	TryOpenAndEndModal();
	EndModal(wxID_OK);
}

void OpenTypeDlg::OnItemActivated(wxListEvent &event)
{
	wxUnusedVar(event);
	TryOpenAndEndModal();
	
}
