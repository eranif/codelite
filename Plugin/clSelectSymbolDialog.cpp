#include "clSelectSymbolDialog.h"
#include "globals.h"
#include <algorithm>

wxDEFINE_EVENT(wxEVT_SYMBOL_SELECTED, clCommandEvent);

clSelectSymbolDialog::clSelectSymbolDialog(wxWindow* parent, const clSelectSymbolDialogEntry::List_t& entries)
    : clSelectSymbolDialogBase(parent)
{
    CenterOnParent();
    std::for_each(entries.begin(), entries.end(), [&](const clSelectSymbolDialogEntry& entry) {
        AddSymbol(entry.name, entry.bmp, entry.help, entry.clientData);
    });
    
    if(m_dvListCtrl->GetItemCount()) {
        m_dvListCtrl->Select(m_dvListCtrl->RowToItem(0));
    }
}

clSelectSymbolDialog::~clSelectSymbolDialog()
{
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxClientData* cd = reinterpret_cast<wxClientData*>(m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(i)));
        wxDELETE(cd);
    }
}

void clSelectSymbolDialog::AddSymbol(const wxString& name,
                                     const wxBitmap& bmp,
                                     const wxString& help,
                                     wxClientData* clientData)
{
    wxVector<wxVariant> cols;
    cols.push_back(::MakeIconText(name, bmp));
    cols.push_back(help);
    m_dvListCtrl->AppendItem(cols, (wxUIntPtr)clientData);
}

void clSelectSymbolDialog::OnOKUI(wxUpdateUIEvent& event) { event.Enable(m_dvListCtrl->GetSelection().IsOk()); }

wxClientData* clSelectSymbolDialog::GetSelection() const
{
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    if(!item) return NULL;

    wxClientData* cd = reinterpret_cast<wxClientData*>(m_dvListCtrl->GetItemData(item));
    return cd;
}
void clSelectSymbolDialog::OnItemActivated(wxDataViewEvent& event) { EndModal(wxID_OK); }
