#include "findresultscontainer.h"
#include "output_pane.h"
#include "findresultstab.h"
//#include "wx/wxFlatNotebook/wxFlatNotebook.h"
//#include "wx/notebook.h"
//#include "wx/toolbook.h"
//#include "imgbook/imagebook.h"

FindResultsContainer::FindResultsContainer(wxWindow *win, wxWindowID id)
		: wxPanel(win, id)
{
	Initialize();
}

FindResultsContainer::~FindResultsContainer()
{
}

void FindResultsContainer::Initialize()
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer(sz);

	//create the toolbar
	long style = wxFNB_NO_X_BUTTON |
	             wxFNB_NO_NAV_BUTTONS |
	             wxFNB_DROPDOWN_TABS_LIST |
				 wxFNB_FF2 | 
				 wxFNB_NODRAG | wxFNB_BOTTOM;

	m_book = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);

	//add 5 tabs for the find results
	FindResultsTab *tab = new FindResultsTab(m_book, wxID_ANY, OutputPane::FIND_IN_FILES_WIN);
	m_book->AddPage(tab, wxT("Find Results 1"), false);

	tab = new FindResultsTab(m_book, wxID_ANY, OutputPane::FIND_IN_FILES_WIN);
	m_book->AddPage(tab, wxT("Find Results 2"), false);

	tab = new FindResultsTab(m_book, wxID_ANY, OutputPane::FIND_IN_FILES_WIN);
	m_book->AddPage(tab, wxT("Find Results 3"), false);

	tab = new FindResultsTab(m_book, wxID_ANY, OutputPane::FIND_IN_FILES_WIN);
	m_book->AddPage(tab, wxT("Find Results 4"), false);

	tab = new FindResultsTab(m_book, wxID_ANY, OutputPane::FIND_IN_FILES_WIN);
	m_book->AddPage(tab, wxT("Find Results 5"), false);

	sz->Add(m_book, 1, wxALL|wxEXPAND);
	sz->Layout();
}

FindResultsTab* FindResultsContainer::GetActiveTab()
{
	int i = m_book->GetSelection();
	if (i == wxNOT_FOUND) {
		return NULL;
	}

	return (FindResultsTab*) m_book->GetPage((size_t)i);
}

void FindResultsContainer::SetActiveTab(int selection)
{
	m_book->SetSelection((size_t)selection);
}
