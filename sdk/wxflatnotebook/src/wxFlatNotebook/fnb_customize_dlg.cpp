#include "wx/wxFlatNotebook/fnb_customize_dlg.h"
#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include "wx/sizer.h"
#include <wx/radiobox.h>
#include <wx/checkbox.h>

///////////////////////////////////////////////////////////////////////////

wxFNBCustomizeDialog::wxFNBCustomizeDialog( wxWindow* parent, long options, int id, wxString title, wxPoint pos, wxSize size, int style ) 
: wxDialog( parent, id, title, pos, size, style )
, m_options(options)
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	SetSizer( mainSizer );

	mainSizer->Add( CreateOptionsPage(), 1, wxEXPAND | wxALL, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline2, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_close = new wxButton( this, wxID_ANY, wxT("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_close, 0, wxALL, 5 );
	
	mainSizer->Add( btnSizer, 0, wxALIGN_CENTER, 5 );
	
	this->Layout();
	GetSizer()->Fit(this);

	ConnectEvents();
}

void wxFNBCustomizeDialog::ConnectEvents()
{
	m_close->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wxFNBCustomizeDialog::OnClose), NULL, this);

	// Connect all checkbox & radiobox controls to the same handler
	if( m_styles ){
		m_styles->Connect(wxID_ANY, wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler(wxFNBCustomizeDialog::OnStyle), NULL, this);
	}

	m_navigationStyle->Connect(wxID_ANY, wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler(wxFNBCustomizeDialog::OnStyle), NULL, this);
	if( m_tabVPosition ){
		m_tabVPosition->Connect(wxID_ANY, wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler(wxFNBCustomizeDialog::OnStyle), NULL, this);
	}

	m_tabBorder->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxFNBCustomizeDialog::OnStyle), NULL, this);
	m_hideCloseButton->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxFNBCustomizeDialog::OnStyle), NULL, this);
	m_mouseMiddleCloseTab->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxFNBCustomizeDialog::OnStyle), NULL, this);
	m_xButtonOnTab->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxFNBCustomizeDialog::OnStyle), NULL, this);
	m_dlbClickCloseTab->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxFNBCustomizeDialog::OnStyle), NULL, this);
	m_smartTabbing->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxFNBCustomizeDialog::OnStyle), NULL, this);

	if( m_allowDragAndDrop ){
		m_allowDragAndDrop->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxFNBCustomizeDialog::OnStyle), NULL, this);
	}

	if( m_foreignDnD ){
		m_foreignDnD->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxFNBCustomizeDialog::OnStyle), NULL, this);
	}

	m_gradient->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxFNBCustomizeDialog::OnStyle), NULL, this);
	m_colorfulTab->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxFNBCustomizeDialog::OnStyle), NULL, this);
}

wxPanel *wxFNBCustomizeDialog::CreateOptionsPage()
{
	wxPageContainer *pc = static_cast<wxPageContainer*>(GetParent());

	wxPanel *page = new wxPanel(this);
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	page->SetSizer( mainSizer );

	wxString m_stylesChoices[] = { wxT("Default"), wxT("Visual Studio 8.0"), wxT("Firefox 2") };
	int m_stylesNChoices = sizeof( m_stylesChoices ) / sizeof( wxString );

	// Set the selected tabs' style
	int selection = 0;
	bool enableGrad = false, enableColorTab = false;
	
	if (pc->HasFlag(wxFNB_FF2) ){
		selection = 2;
		enableGrad = true; enableColorTab = false;
	}else if( pc->HasFlag(wxFNB_VC8) ){
		selection = 1;
		enableGrad = true; enableColorTab = true;
	}else {
		selection = 0;
		enableGrad = true; enableColorTab = false;
	}

	// can we customize the tab's style?
	if( m_options & wxFNB_CUSTOM_TAB_LOOK ){
		m_styles = new wxRadioBox( page, wxID_ANY, wxT("Visual Style:"), wxDefaultPosition, wxDefaultSize, m_stylesNChoices, m_stylesChoices, 3, wxRA_SPECIFY_COLS );
		m_styles->SetSelection(selection);
		mainSizer->Add( m_styles, 0, wxALL|wxEXPAND, 5 );
	}else{
		m_styles = NULL;
	}

	// can we customize the tab's orientation?
	if( m_options & wxFNB_CUSTOM_ORIENTATION ){
		wxString m_tabVPositionChoices[] = { wxT("Upper Tabs"), wxT("Bottom Tabs") };
		int m_tabVPositionNChoices = sizeof( m_tabVPositionChoices ) / sizeof( wxString );
		m_tabVPosition = new wxRadioBox( page, wxID_ANY, wxT("Tab Positioning:"), wxDefaultPosition, wxDefaultSize, m_tabVPositionNChoices, m_tabVPositionChoices, 2, wxRA_SPECIFY_COLS );
		m_tabVPosition->SetSelection(pc->HasFlag(wxFNB_BOTTOM) ? 1 : 0);
		mainSizer->Add( m_tabVPosition, 0, wxALL|wxEXPAND, 5 );
	}else{
		m_tabVPosition = NULL;
	}

	// there is no harm in showing this always
	wxString navigationStyle [] = { wxT("Rotation Arrows"), wxT("Drop Down Tab List"), wxT("None") };
	int nChoices = sizeof( navigationStyle ) / sizeof( wxString );
	m_navigationStyle = new wxRadioBox( page, wxID_ANY, wxT("Navigation Style:"), wxDefaultPosition, wxDefaultSize, nChoices, navigationStyle, 3, wxRA_SPECIFY_COLS );

	if(pc->HasFlag(wxFNB_DROPDOWN_TABS_LIST)){
		m_navigationStyle->SetSelection(1);
	} else if(pc->HasFlag(wxFNB_NO_NAV_BUTTONS) && !pc->HasFlag(wxFNB_DROPDOWN_TABS_LIST)){
		m_navigationStyle->SetSelection(2);
	} else {
		m_navigationStyle->SetSelection(0);
	}

	mainSizer->Add( m_navigationStyle, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( page, -1, wxT("General:") ), wxVERTICAL );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 3, 2, 0, 0 );
	
	m_tabBorder = new wxCheckBox( page, wxID_ANY, wxT("Draw Thin Border Around Tab Area"), wxDefaultPosition, wxDefaultSize, 0 );
	m_tabBorder->SetValue(pc->HasFlag(wxFNB_TABS_BORDER_SIMPLE));
	gSizer1->Add( m_tabBorder, 0, wxALL, 5 );
	
	// do we allow customizing close button options? ( in all forms: dclick, middle click, on tab and right side)
	m_hideCloseButton = new wxCheckBox( page, wxID_ANY, wxT("Hide Close Button"), wxDefaultPosition, wxDefaultSize, 0 );
	m_hideCloseButton->SetValue(pc->HasFlag(wxFNB_NO_X_BUTTON));
	m_hideCloseButton->Enable(m_options & wxFNB_CUSTOM_CLOSE_BUTTON ? true : false);
	gSizer1->Add( m_hideCloseButton, 0, wxALL, 5 );

	m_mouseMiddleCloseTab = new wxCheckBox( page, wxID_ANY, wxT("Mouse Middle Button Closes Tab"), wxDefaultPosition, wxDefaultSize, 0 );
	m_mouseMiddleCloseTab->SetValue(pc->HasFlag(wxFNB_MOUSE_MIDDLE_CLOSES_TABS));
	m_mouseMiddleCloseTab->Enable(m_options & wxFNB_CUSTOM_CLOSE_BUTTON ? true : false);
	gSizer1->Add( m_mouseMiddleCloseTab, 0, wxALL, 5 );

	m_xButtonOnTab = new wxCheckBox( page, wxID_ANY, wxT("Place Close Button On Active Tab"), wxDefaultPosition, wxDefaultSize, 0 );
	m_xButtonOnTab->SetValue(pc->HasFlag(wxFNB_X_ON_TAB));
	m_xButtonOnTab->Enable(m_options & wxFNB_CUSTOM_CLOSE_BUTTON ? true : false);
	gSizer1->Add( m_xButtonOnTab, 0, wxALL, 5 );

	m_dlbClickCloseTab = new wxCheckBox( page, wxID_ANY, wxT("Double Click Closes Tab"), wxDefaultPosition, wxDefaultSize, 0 );
	m_dlbClickCloseTab->SetValue(pc->HasFlag(wxFNB_DCLICK_CLOSES_TABS));
	m_dlbClickCloseTab->Enable(m_options & wxFNB_CUSTOM_CLOSE_BUTTON ? true : false);
	gSizer1->Add( m_dlbClickCloseTab, 0, wxALL, 5 );

	m_smartTabbing = new wxCheckBox( page, wxID_ANY, wxT("Enable Smart Tabbing"), wxDefaultPosition, wxDefaultSize, 0 );
	m_smartTabbing->SetValue(pc->HasFlag(wxFNB_SMART_TABS));
	gSizer1->Add( m_smartTabbing, 0, wxALL, 5 );
	
	sbSizer2->Add( gSizer1, 0, wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer2, 0, wxALL|wxEXPAND, 5 );
	
	if(m_options & wxFNB_CUSTOM_LOCAL_DRAG){
		wxStaticBoxSizer* sbSizer4;
		sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( page, -1, wxT("Drag And Drop:") ), wxHORIZONTAL );
		m_allowDragAndDrop = new wxCheckBox( page, wxID_ANY, wxT("Enable Drag And Drop"), wxDefaultPosition, wxDefaultSize, 0 );
		m_allowDragAndDrop->SetValue(!pc->HasFlag(wxFNB_NODRAG));

		sbSizer4->Add( m_allowDragAndDrop, 0, wxALL, 5 );

		m_foreignDnD = new wxCheckBox( page, wxID_ANY, wxT("Allow Drag And Drop from other notebooks"), wxDefaultPosition, wxDefaultSize, 0 );
		m_foreignDnD->SetValue(pc->HasFlag(wxFNB_ALLOW_FOREIGN_DND));
		m_foreignDnD->Enable(m_allowDragAndDrop->IsChecked() && m_options & wxFNB_CUSTOM_FOREIGN_DRAG);
		sbSizer4->Add( m_foreignDnD, 0, wxALL, 5 );
		mainSizer->Add( sbSizer4, 0, wxALL|wxEXPAND, 5 );

	}else{
		m_allowDragAndDrop = NULL;
		m_foreignDnD = NULL;
	}	

	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( page, -1, wxT("Effects:") ), wxHORIZONTAL );
	
	m_gradient = new wxCheckBox( page, wxID_ANY, wxT("Use Gradient painting for tabs area"), wxDefaultPosition, wxDefaultSize, 0 );
	m_gradient->SetValue(pc->HasFlag(wxFNB_BACKGROUND_GRADIENT));
	sbSizer5->Add( m_gradient, 0, wxALL, 5 );
	
	m_colorfulTab = new wxCheckBox( page, wxID_ANY, wxT("Set Random Colour for Tabs"), wxDefaultPosition, wxDefaultSize, 0 );
	m_colorfulTab->SetValue(pc->HasFlag(wxFNB_COLORFUL_TABS));
	sbSizer5->Add( m_colorfulTab, 0, wxALL, 5 );
	
	mainSizer->Add( sbSizer5, 0, wxALL|wxEXPAND, 5 );
	m_colorfulTab->Enable(enableColorTab);
	m_gradient->Enable(enableGrad);
	page->Layout();
	return page;
}

//---------------------------------------------------------
// Event handlers
//---------------------------------------------------------

void wxFNBCustomizeDialog::OnClose(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(wxID_OK);
}

void wxFNBCustomizeDialog::OnStyle(wxCommandEvent &event)
{
	wxObject *obj = event.GetEventObject();
	wxPageContainer *pc = static_cast<wxPageContainer*>(GetParent());

	wxFlatNotebook *bookWin = static_cast<wxFlatNotebook*>( pc->GetParent() );
	long style = bookWin->GetWindowStyleFlag();

	// Handle tab style
	if( obj == m_styles ){
		style &= ~(wxFNB_VC8);
		style &= ~(wxFNB_FF2);
		
		if(event.GetSelection() == 1){	// vc8
			// remove bottom flag if exist
			style |= wxFNB_VC8;
			m_gradient->Enable(true);
			m_colorfulTab->Enable(true);
		}else if(event.GetSelection() == 2){	// ff2
			style |= wxFNB_FF2;
			m_gradient->Enable(true);
			m_colorfulTab->Enable(false);
		} else {
			// by default this two options are not available
			m_gradient->Enable(false);
			m_colorfulTab->Enable(false);
		}
	}

	// Handle tab postioninig
	if( obj == m_tabVPosition ){
		if(event.GetSelection() == 0){	// Top
			// remove bottom flag if exist
			style &= ~(wxFNB_BOTTOM);
		} else {
			style |= wxFNB_BOTTOM;
		}
	}

	// Handle navigation style
	if( obj == m_navigationStyle ){
		if(event.GetSelection() == 0){	// Arrows
			style &= ~wxFNB_DROPDOWN_TABS_LIST;
			style &= ~wxFNB_NO_NAV_BUTTONS;
		} else if(event.GetSelection() == 1){ // list
			style |= wxFNB_NO_NAV_BUTTONS;
			style |= wxFNB_DROPDOWN_TABS_LIST;
		} else {
			// none
			style |= wxFNB_NO_NAV_BUTTONS;
			style &= ~wxFNB_DROPDOWN_TABS_LIST;
		}
	}

	// Simple tab border around tab area
	if(obj == m_tabBorder){
		if(event.IsChecked()){
			style |= wxFNB_TABS_BORDER_SIMPLE;
		} else {
			style &= ~(wxFNB_TABS_BORDER_SIMPLE);
		}
	}

	// Hide close button
	if(obj == m_hideCloseButton){
		if(event.IsChecked()){
			style |= wxFNB_NO_X_BUTTON;
		} else {
			style &= ~(wxFNB_NO_X_BUTTON);
		}
	}

	if(obj == m_mouseMiddleCloseTab){
		if(event.IsChecked()){
			style |= wxFNB_MOUSE_MIDDLE_CLOSES_TABS;
		} else {
			style &= ~(wxFNB_MOUSE_MIDDLE_CLOSES_TABS);
		}
	}

	if(obj == m_xButtonOnTab){
		if(event.IsChecked()){
			style |= wxFNB_X_ON_TAB;
		} else {
			style &= ~(wxFNB_X_ON_TAB);
		}
	}

	if(obj == m_dlbClickCloseTab){
		if(event.IsChecked()){
			style |= wxFNB_DCLICK_CLOSES_TABS;
		} else {
			style &= ~(wxFNB_DCLICK_CLOSES_TABS);
		}
	}

	if(obj == m_smartTabbing){
		if(event.IsChecked()){
			style |= wxFNB_SMART_TABS;
		} else {
			style &= ~(wxFNB_SMART_TABS);
		}
	}
	
	if(obj == m_allowDragAndDrop){
		if(event.IsChecked()){
			m_foreignDnD->Enable(m_options & wxFNB_CUSTOM_FOREIGN_DRAG ? true : false);
			style &= ~wxFNB_NODRAG;
		} else {
			m_foreignDnD->Enable(false);
			style |= wxFNB_NODRAG;
		}
	}

	if(obj == m_foreignDnD){
		if(event.IsChecked()){
			style |= wxFNB_ALLOW_FOREIGN_DND;
		} else {
			style &= ~wxFNB_ALLOW_FOREIGN_DND;
		}
	}

	if(obj == m_gradient){
		if(event.IsChecked()){
			style |= wxFNB_BACKGROUND_GRADIENT;
		} else {
			style &= ~wxFNB_BACKGROUND_GRADIENT;
		}
	}

	if(obj == m_colorfulTab){
		if(event.IsChecked()){
			style |= wxFNB_COLORFUL_TABS;
		} else {
			style &= ~wxFNB_COLORFUL_TABS;
		}
	}

	bookWin->SetWindowStyleFlag(style);
	bookWin->Refresh();
}


