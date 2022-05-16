#include "clPropertiesPage.hpp"

#include "clThemedListCtrl.h"

clPropertiesPage::clPropertiesPage(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));

    m_view = new clThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_NO_HEADER | wxDV_ROW_LINES);
    GetSizer()->Add(m_view, wxSizerFlags(1).Expand());
    m_view->AppendTextColumn(_("Name"));
    m_view->AppendTextColumn(_("Value"));
    GetSizer()->Layout();
}

clPropertiesPage::~clPropertiesPage()
{
    m_view->DeleteAllItems([](wxUIntPtr d) {
        wxArrayString* choices = reinterpret_cast<wxArrayString*>(d);
        wxDELETE(choices);
    });
}

void clPropertiesPage::AddProperty(const wxString& label, const wxArrayString& choices, size_t sel)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    clDataViewButton choice(sel < choices.size() ? choices[sel] : wxString(), wxT("\u25BC"), wxNOT_FOUND);
    wxVariant v;
    v << choice;
    cols.push_back(v);
    m_view->AppendItem(cols, (wxUIntPtr) new wxArrayString(choices));
}

void clPropertiesPage::AddProperty(const wxString& label, bool checked)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    clDataViewCheckbox c(checked);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
}

void clPropertiesPage::AddProperty(const wxString& label, const wxString& value)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    clDataViewButton c(value, wxT("\u22EF"), wxNOT_FOUND);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
}

void clPropertiesPage::AddProperty(const wxString& label, const wxColour& value)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    clDataViewColour c(value);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
}
