///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "pluginwizard_page1.h"
#include "wx/msgdlg.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(PluginWizardPage1, wxWizardPageSimple)
EVT_WIZARD_PAGE_CHANGING(wxID_ANY, PluginWizardPage1::OnValidate)
END_EVENT_TABLE()

PluginWizardPage1::PluginWizardPage1( wxWizard* parent) 
: wxWizardPageSimple(parent)
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("General Information:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer1->Add( m_staticText5, 0, wxALL, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Plugin Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlPluginName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_textCtrlPluginName, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Description:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlDescription = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_textCtrlDescription, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

void PluginWizardPage1::OnValidate(wxWizardEvent &event)
{
	wxString name = GetName();
	name = name.Trim().Trim(false);
	
	//we dont accept empty plugin names
	if(name.IsEmpty()){
		wxMessageBox(wxT("Missing plugin name"), wxT("CodeLite"), wxOK | wxICON_WARNING);
		event.Veto();
		return;
	}
	//a valid name must contains only 
	//[A-Za-z_]
	if(name.find_first_not_of(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_")) != wxString::npos){
		wxMessageBox(wxT("Invalid characters in plugin name\nonly [A-Za-z_] are allowed"), wxT("CodeLite"), wxOK | wxICON_WARNING);
		event.Veto();
		return;
	}
	event.Skip();
}
