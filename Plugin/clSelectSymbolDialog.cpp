#include "clSelectSymbolDialog.h"

#include "ColoursAndFontsManager.h"
#include "globals.h"

#include <algorithm>

wxDEFINE_EVENT(wxEVT_SYMBOL_SELECTED, clCommandEvent);

clSelectSymbolDialog::clSelectSymbolDialog(wxWindow* parent, const clSelectSymbolDialogEntry::List_t& entries)
    : clSelectSymbolDialogBase(parent)
{
    Initialise(entries);
    ::clSetDialogBestSizeAndPosition(this);
}

clSelectSymbolDialog::~clSelectSymbolDialog()
{
    m_dvListCtrl->DeleteAllItems([](wxUIntPtr p) {
        auto d = reinterpret_cast<wxClientData*>(p);
        wxDELETE(d);
    });
}

void clSelectSymbolDialog::AddSymbol(const wxString& name, const wxBitmap& bmp, const wxString& help,
                                     wxClientData* clientData)
{
    wxVector<wxVariant> cols;
    wxUnusedVar(help);
    cols.push_back(::MakeIconText(name, bmp));
    m_dvListCtrl->AppendItem(cols, (wxUIntPtr)clientData);
}

void clSelectSymbolDialog::OnOKUI(wxUpdateUIEvent& event) { event.Enable(m_dvListCtrl->GetSelection().IsOk()); }

wxClientData* clSelectSymbolDialog::GetSelection() const
{
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    if(!item)
        return NULL;

    wxClientData* cd = reinterpret_cast<wxClientData*>(m_dvListCtrl->GetItemData(item));
    return cd;
}
void clSelectSymbolDialog::OnItemActivated(wxDataViewEvent& event) { EndModal(wxID_OK); }

void clSelectSymbolDialog::Initialise(const clSelectSymbolDialogEntry::List_t& entries)
{
    m_dvListCtrl->DeleteAllItems([](wxUIntPtr p) {
        auto d = reinterpret_cast<wxClientData*>(p);
        wxDELETE(d);
    });

    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    wxFont f = lexer->GetFontForStyle(0, m_dvListCtrl);
    m_dvListCtrl->SetDefaultFont(f);
    m_dvListCtrl->Begin();
    for(const auto& entry : entries) {
        AddSymbol(entry.name, entry.bmp, entry.help, entry.clientData);
    }

    m_dvListCtrl->Commit();
    if(m_dvListCtrl->GetItemCount()) {
        m_dvListCtrl->Select(m_dvListCtrl->RowToItem(0));
    }
}