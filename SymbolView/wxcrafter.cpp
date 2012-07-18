#include "wxcrafter.h"


// Declare the bitmap loading function
extern void wxC682BInitBitmapResources();

static bool bBitmapLoaded = false;


SymbolViewTabPanelBaseClass::SymbolViewTabPanelBaseClass(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC682BInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer1 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer1);
    
    m_textCtrlSearch = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), wxTE_RICH2|wxTE_PROCESS_ENTER);
    m_textCtrlSearch->SetToolTip(_("Search a symbol"));
    
    boxSizer1->Add(m_textCtrlSearch, 0, wxLEFT|wxRIGHT|wxTOP|wxBOTTOM|wxEXPAND, 2);
    
    
    GetSizer()->Fit(this);
    Centre();
    // Connect events
    m_textCtrlSearch->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(SymbolViewTabPanelBaseClass::OnSearchSymbol), NULL, this);
    m_textCtrlSearch->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(SymbolViewTabPanelBaseClass::OnSearchEnter), NULL, this);
    
}

SymbolViewTabPanelBaseClass::~SymbolViewTabPanelBaseClass()
{
    m_textCtrlSearch->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(SymbolViewTabPanelBaseClass::OnSearchSymbol), NULL, this);
    m_textCtrlSearch->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(SymbolViewTabPanelBaseClass::OnSearchEnter), NULL, this);
    
}
