#include "findresultscontainer.h"
#include "output_pane.h"
#include "findresultstab.h"
#include "custom_notebook.h"

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

	m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_BOTTOM);

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
