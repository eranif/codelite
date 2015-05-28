#include "NotebookNavigationDlg.h"
#include <list>
#include "Notebook.h"
#include <algorithm>
#include "globals.h"

struct TabData {
    wxString label;
    wxBitmap bmp;
    int index;
};

NotebookNavigationDlg::NotebookNavigationDlg(wxWindow* parent, Notebook* book)
    : NotebookNavigationDlgBase(parent)
    , m_book(book)
{
    clTabHistory::Ptr_t history = m_book->GetHistory();
    const std::list<wxWindow*>& windows = history->GetHistory();
    // Populate the list
    std::for_each(windows.begin(), windows.end(), [&](wxWindow* page) {
        int index = m_book->FindPage(page);
        if(index != wxNOT_FOUND) {
            wxString label = m_book->GetPageText(index);
            wxBitmap bmp = m_book->GetPageBitmap(index);

            wxVector<wxVariant> cols;
            cols.push_back(::MakeIconText(label, bmp));
            TabData* d = new TabData;
            d->bmp = bmp;
            d->label = label;
            d->index = index;
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr)d);
        }
    });
}

NotebookNavigationDlg::~NotebookNavigationDlg() {}

void NotebookNavigationDlg::CloseDialog()
{
    wxDataViewItem selection = m_dvListCtrl->GetSelection();
    if(selection.IsOk()) {
        TabData* d = (TabData*)m_dvListCtrl->GetItemData(selection);
        m_book->SetSelection(d->index);
    }
    EndModal(wxID_OK);
}

void NotebookNavigationDlg::OnKeyDown(wxKeyEvent& event) 
{
    // Change the selection
}

void NotebookNavigationDlg::OnKeyUp(wxKeyEvent& event)
{
    if(event.GetKeyCode() == WXK_CONTROL) {
        CloseDialog();
    } else {
        event.Skip();
    }
}
