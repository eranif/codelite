#include "DbViewerPanel.h"
#include "ViewSettings.h"

ViewSettings::ViewSettings( wxWindow* parent,IDbAdapter* pDbAdapter):_ViewSettings(parent)
{
	DbViewerPanel::InitStyledTextCtrl( m_scintilla2 );
	m_pDbAdapter = pDbAdapter;
}

ViewSettings::~ViewSettings()
{
}

void ViewSettings::OnOKClick(wxCommandEvent& event)
{
	m_pView->SetName(m_txName->GetValue());
	m_pView->SetSelect(m_scintilla2->GetText());
	Close();
}
void ViewSettings::SetView(View* pView, wxSFDiagramManager* pManager)
{
	m_pView = pView;
	m_pDiagramManager = pManager;
	
	m_txName->SetValue(pView->GetName());
	m_scintilla2->SetText(pView->GetSelect());	
}

