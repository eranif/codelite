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

#include "ext_db_page1.h"
#include "ext_db_page2.h"
#include "wx/filename.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(ExtDbPage1, wxWizardPageSimple)
    EVT_WIZARD_PAGE_CHANGING(wxID_ANY, ExtDbPage1::OnValidate)
END_EVENT_TABLE()

ExtDbPage1::ExtDbPage1(wxWizard* parent)
        : wxWizardPageSimple(parent)
{
    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer( wxVERTICAL );

    m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Please select the root path to your include files:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText1->Wrap( -1 );
    bSizer1->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );

    m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer2;
    bSizer2 = new wxBoxSizer( wxVERTICAL );

    m_dirPicker = new DirPicker(m_panel1);
    bSizer2->Add( m_dirPicker, 0, wxALL|wxEXPAND, 5 );

    m_panel1->SetSizer( bSizer2 );
    m_panel1->Layout();
    bSizer2->Fit( m_panel1 );
    bSizer1->Add( m_panel1, 1, wxEXPAND | wxALL, 0 );

    wxStaticBoxSizer* sbSizer1;
    sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxEmptyString ), wxVERTICAL );

    m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("CodeLite searches this path for files that matches\nthe file type pattern as provided in the ctags options dialog\n"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText3->Wrap( -1 );
    sbSizer1->Add( m_staticText3, 1, wxALL|wxEXPAND, 5 );

    bSizer1->Add( sbSizer1, 0, wxEXPAND, 5 );

    this->SetSizer( bSizer1 );
    this->Layout();
}

void ExtDbPage1::OnValidate(wxWizardEvent &event)
{
    //validate that there is a valid name to the database
    wxFileName path(m_dirPicker->GetPath());
    if (!path.DirExists()){
        wxMessageBox(wxT("Invalid input directory"));
        event.Veto();
        return;
    }

    ExtDbPage2* nextPage = (ExtDbPage2*)GetNext();
    nextPage->BuildTree(m_dirPicker->GetPath());
}
