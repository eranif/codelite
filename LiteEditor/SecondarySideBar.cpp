#include "SecondarySideBar.hpp"

SecondarySideBar::SecondarySideBar(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));

    m_book = new clSideBarCtrl(this);
    GetSizer()->Add(m_book, wxSizerFlags().Expand().Proportion(1));
    m_book->SetOrientationOnTheRight(true);
    GetSizer()->Fit(this);
}

SecondarySideBar::~SecondarySideBar() {}
