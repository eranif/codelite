///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 28 2007)
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

#include "ext_db_page2.h"
#include "macros.h"
#include "checkdirtreectrl.h"

///////////////////////////////////////////////////////////////////////////

ExtDbPage2::ExtDbPage2(wxWizard* parent)
: wxWizardPageSimple(parent)
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Select the directories to be parsed:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer1->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_includeDirs = new CheckDirTreeCtrl( m_panel1, wxEmptyString, wxID_ANY);
	bSizer2->Add(m_includeDirs, 1, wxEXPAND | wxALL, 5 );
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND | wxALL, 0 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxEmptyString ), wxVERTICAL );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("TIP: To make the database as small as possible (i.e. better performance),\nSelect only the directories that contains your symbols"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	sbSizer1->Add( m_staticText3, 1, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( sbSizer1, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();

}

void ExtDbPage2::GetIncludeDirs(wxArrayString &arr)
{
	m_includeDirs->GetSelectedDirs(arr);
}

void ExtDbPage2::BuildTree(const wxString &path)
{
	m_includeDirs->BuildTree(path);
}
