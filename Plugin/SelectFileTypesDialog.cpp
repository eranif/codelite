#include "SelectFileTypesDialog.hpp"

#include "globals.h"
#include "windowattrmanager.h"

SelectFileTypesDialog::SelectFileTypesDialog(wxWindow* parent)
    : SelectFileTypesDialogBase(parent)
{
    m_all = FileExtManager::GetAllSupportedFileTypes();
    InitializeList();

    // set the initial size
    clSetSmallDialogBestSizeAndPosition(this);

    // load any old size from previous activation
    WindowAttrManager::Load(this);

    // but always center on parent
    CentreOnParent();

    m_searchCtrl->CallAfter(&wxSearchCtrl::SetFocus);
}

SelectFileTypesDialog::~SelectFileTypesDialog() {}

void SelectFileTypesDialog::InitializeList()
{
    m_dvListCtrl->Begin();
    wxVector<wxVariant> cols;
    for(const auto& vt : m_all) {
        // include this entry
        clDataViewCheckbox item(false, wxNOT_FOUND, vt.first);
        wxVariant v;
        v << item;
        cols.push_back(v);
        m_dvListCtrl->AppendItem(cols);
    }
    m_dvListCtrl->Commit();
}

void SelectFileTypesDialog::Search(const wxString& filter)
{
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxString text = m_dvListCtrl->GetItemText(m_dvListCtrl->RowToItem(i));
        if(text.Contains(filter)) {
            m_dvListCtrl->SelectRow(i);
            break;
        }
    }
}

std::unordered_map<wxString, FileExtManager::FileType> SelectFileTypesDialog::GetValue() const
{
    // collect checked items
    std::unordered_map<wxString, FileExtManager::FileType> result;
    for(size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        auto item = m_dvListCtrl->RowToItem(i);
        wxString text = m_dvListCtrl->GetItemText(item);
        if(m_dvListCtrl->IsItemChecked(item) && m_all.count(text)) {
            auto entry = m_all.find(text);
            result.insert({ entry->first, entry->second });
        }
    }
    return result;
}
