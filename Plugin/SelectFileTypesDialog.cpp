#include "SelectFileTypesDialog.hpp"

#include "globals.h"
#include "windowattrmanager.h"

SelectFileTypesDialog::SelectFileTypesDialog(wxWindow* parent, const wxArrayString& initial_selection)
    : SelectFileTypesDialogBase(parent)
{
    m_all = FileExtManager::GetLanguageBundles();
    InitializeList(initial_selection);

    // set the initial size
    clSetSmallDialogBestSizeAndPosition(this);

    // load any old size from previous activation
    WindowAttrManager::Load(this);

    // but always center on parent
    CentreOnParent();

    m_searchCtrl->CallAfter(&wxSearchCtrl::SetFocus);
}

SelectFileTypesDialog::~SelectFileTypesDialog() {}

void SelectFileTypesDialog::InitializeList(const wxArrayString& selected_items)
{
    m_dvListCtrl->DeleteAllItems();
    wxVector<wxVariant> cols;
    for(const auto& vt : m_all) {
        // include this entry
        clDataViewCheckbox item(selected_items.Index(vt.first) != wxNOT_FOUND, wxNOT_FOUND, vt.first);
        wxVariant v;
        v << item;
        cols.push_back(v);
        m_dvListCtrl->AppendItem(cols);
        cols.clear();
    }
}

void SelectFileTypesDialog::Search(const wxString& filter)
{
    if(filter.empty()) {
        return;
    }
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        auto item = m_dvListCtrl->RowToItem(i);
        wxString text = m_dvListCtrl->GetItemText(item);
        if(text.Contains(filter)) {
            m_dvListCtrl->SelectRow(i);
            m_dvListCtrl->EnsureVisible(item);
            break;
        }
    }
}

wxArrayString SelectFileTypesDialog::GetValue() const
{
    // collect checked items
    wxArrayString result;
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        auto item = m_dvListCtrl->RowToItem(i);
        if(m_dvListCtrl->IsItemChecked(item)) {
            wxString text = m_dvListCtrl->GetItemText(item);
            result.Add(text);
        }
    }
    return result;
}

void SelectFileTypesDialog::OnSearch(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Search(m_searchCtrl->GetValue());
}
