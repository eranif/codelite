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
    ::clSetSmallDialogBestSizeAndPosition(this);
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
        wxStringClientData* cd = (wxStringClientData*)m_dvListCtrl->GetItemData(item);
        return cd->GetData();
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
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxStringClientData* cd = (wxStringClientData*)m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(i));
        wxDELETE(cd);
    }
    m_dvListCtrl->DeleteAllItems();

    wxString filter = m_searchCtrl->GetValue();
    for(size_t i = 0; i < m_options.size(); ++i) {
        if(FileUtils::FuzzyMatch(filter, m_options.Item(i))) {
            wxVector<wxVariant> cols;
            wxString displayString = m_options.Item(i).BeforeFirst('\n');
            displayString.Trim().Trim(false);
            cols.push_back(displayString);
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr)(new wxStringClientData(m_options.Item(i))));
        }
    }
}
