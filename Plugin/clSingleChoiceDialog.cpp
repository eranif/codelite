#include "clSingleChoiceDialog.h"
#include "globals.h"
#include "fileutils.h"

clSingleChoiceDialog::clSingleChoiceDialog(wxWindow* parent, const wxArrayString& options, int initialSelection)
    : clSingleChoiceDialogBase(parent)
    , m_options(options)
{
    DoInitialise();
    if(initialSelection >= 0 && initialSelection < (int)options.size()) {
        m_dvListCtrl->Select(m_dvListCtrl->RowToItem(initialSelection));
    }
    CenterOnParent();
}

clSingleChoiceDialog::~clSingleChoiceDialog() {}

void clSingleChoiceDialog::OnOKUI(wxUpdateUIEvent& event)
{
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    event.Enable(item.IsOk());
}

wxString clSingleChoiceDialog::GetSelection() const
{
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    if(item.IsOk()) {
        wxVariant v;
        m_dvListCtrl->GetValue(v, m_dvListCtrl->ItemToRow(item), 0);
        return v.GetString();
    }
    return "";
}

void clSingleChoiceDialog::OnItemActivated(wxCommandEvent& event) { EndModal(wxID_OK); }
void clSingleChoiceDialog::OnActivated(wxDataViewEvent& event) { EndModal(wxID_OK); }
void clSingleChoiceDialog::OnFilter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoInitialise();
}

void clSingleChoiceDialog::DoInitialise()
{
    m_dvListCtrl->DeleteAllItems();
    wxString filter = m_searchCtrl->GetValue();
    for(size_t i = 0; i < m_options.size(); ++i) {
        if(FileUtils::FuzzyMatch(filter, m_options.Item(i))) {
            wxVector<wxVariant> cols;
            cols.push_back(m_options.Item(i));
            m_dvListCtrl->AppendItem(cols);
        }
    }
}
