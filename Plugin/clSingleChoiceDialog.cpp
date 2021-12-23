#include "clSingleChoiceDialog.h"

#include "fileutils.h"
#include "globals.h"

clSingleChoiceDialog::clSingleChoiceDialog(wxWindow* parent, const wxArrayString& options, int initialSelection)
    : clSingleChoiceDialogBase(parent)
    , m_options(options)
{
    DoInitialise();
    if(initialSelection >= 0 && initialSelection < (int)options.size()) {
        m_dvListCtrl->Select(m_dvListCtrl->RowToItem(initialSelection));
        m_dvListCtrl->EnsureVisible(m_dvListCtrl->RowToItem(initialSelection));
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

void clSingleChoiceDialog::OnActivated(wxDataViewEvent& event) { EndModal(wxID_OK); }

void clSingleChoiceDialog::DoInitialise()
{
    m_dvListCtrl->DeleteAllItems([](wxUIntPtr d) -> void {
        wxStringClientData* cd = reinterpret_cast<wxStringClientData*>(d);
        wxDELETE(cd);
    });

    for(size_t i = 0; i < m_options.size(); ++i) {
        wxVector<wxVariant> cols;
        wxString displayString = m_options.Item(i).BeforeFirst('\n');
        displayString.Trim().Trim(false);
        cols.push_back(displayString);
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)(new wxStringClientData(m_options.Item(i))));
    }
}
