#include "NotebookNavigationDlg.h"

#include "Notebook.h"
#include "file_logger.h"
#include "windowattrmanager.h"

#include <imanager.h>
#include <wx/app.h>
#include <wx/dynarray.h>

NotebookNavigationDlg::NotebookNavigationDlg(wxWindow* parent, Notebook* book)
    : NotebookNavigationDlgBase(parent)
    , m_notebook(book)
    , m_selection(wxNOT_FOUND)
{
    Initialise(m_notebook);
    FinalizeCtor();
}

NotebookNavigationDlg::NotebookNavigationDlg(wxWindow* parent, clAuiBook* book)
    : NotebookNavigationDlgBase(parent)
    , m_aui_notebook(book)
    , m_selection(wxNOT_FOUND)
{
    Initialise(m_aui_notebook);
    FinalizeCtor();
}

NotebookNavigationDlg::~NotebookNavigationDlg()
{
#if defined(__WXMSW__) || defined(__WXMAC__)
    UnregisterHotKey(XRCID("tab-next-entry"));
    UnregisterHotKey(XRCID("tab-prev-entry"));
    wxTheApp->Unbind(wxEVT_HOTKEY, &NotebookNavigationDlg::OnHotKeyNext, this, XRCID("tab-next-entry"));
    wxTheApp->Unbind(wxEVT_HOTKEY, &NotebookNavigationDlg::OnHotKeyPrev, this, XRCID("tab-prev-entry"));
#else
    m_dvListCtrl->Unbind(wxEVT_KEY_DOWN, &NotebookNavigationDlg::OnKeyDown, this);
#endif
    m_dvListCtrl->Unbind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &NotebookNavigationDlg::OnItemActivated, this);
    m_dvListCtrl->Unbind(wxEVT_KEY_UP, &NotebookNavigationDlg::OnKeyUp, this);

    m_dvListCtrl->DeleteAllItems([](wxUIntPtr d) {
        TabData* cd = (TabData*)d;
        wxDELETE(cd);
    });
}

void NotebookNavigationDlg::CloseDialog()
{
    LOG_IF_TRACE { clDEBUG1() << "NotebookNavigationDlg::CloseDialog" << endl; }
    wxDataViewItem selection = m_dvListCtrl->GetSelection();
    if (selection.IsOk()) {
        TabData* d = (TabData*)m_dvListCtrl->GetItemData(selection);
        m_selection = d->index;
    }
    EndModal(wxID_OK);
}

void NotebookNavigationDlg::FinalizeCtor()
{
    if (m_dvListCtrl->GetItemCount() > 1) {
        m_dvListCtrl->Select(m_dvListCtrl->RowToItem(1));
    } else if (m_dvListCtrl->GetItemCount() == 1) {
        m_dvListCtrl->Select(m_dvListCtrl->RowToItem(0));
    }

    SetMinClientSize(wxSize(500, 300));
#ifdef __WXOSX__
    SetSize(wxSize(500, 300));
#endif

#if defined(__WXMSW__) || defined(__WXMAC__)
    RegisterHotKey(XRCID("tab-next-entry"), wxMOD_RAW_CONTROL, WXK_TAB);
    RegisterHotKey(XRCID("tab-prev-entry"), wxMOD_RAW_CONTROL | wxMOD_SHIFT, WXK_TAB);
    wxTheApp->Bind(wxEVT_HOTKEY, &NotebookNavigationDlg::OnHotKeyNext, this, XRCID("tab-next-entry"));
    wxTheApp->Bind(wxEVT_HOTKEY, &NotebookNavigationDlg::OnHotKeyPrev, this, XRCID("tab-prev-entry"));
#else
    m_dvListCtrl->Bind(wxEVT_KEY_DOWN, &NotebookNavigationDlg::OnKeyDown, this);
#endif
    m_dvListCtrl->Bind(wxEVT_KEY_UP, &NotebookNavigationDlg::OnKeyUp, this);
    m_dvListCtrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &NotebookNavigationDlg::OnItemActivated, this);

    m_dvListCtrl->CallAfter(&clThemedListCtrl::SetFocus);
    WindowAttrManager::Load(this);
    CentreOnParent();
}

void NotebookNavigationDlg::OnKeyDown(wxKeyEvent& event)
{
    if ((event.GetKeyCode() == WXK_TAB) && (event.RawControlDown() && event.ShiftDown())) {
        SelectPrev();

    } else if ((event.GetKeyCode() == WXK_TAB) && event.RawControlDown()) {
        SelectNext();

    } else {
        event.Skip();
    }
}

void NotebookNavigationDlg::OnKeyUp(wxKeyEvent& event)
{
    // if CTRL key is down, dismiss the dialog
    if (!wxGetKeyState(WXK_RAW_CONTROL)) {
        CloseDialog();
    } else {
        event.Skip();
    }
}

void NotebookNavigationDlg::OnItemActivated(wxDataViewEvent& event)
{
    event.Skip();
    LOG_IF_TRACE { clDEBUG1() << "NotebookNavigationDlg::OnItemActivated" << endl; }
    CloseDialog();
}

void NotebookNavigationDlg::OnHotKeyNext(wxKeyEvent& event)
{
    wxUnusedVar(event);
    SelectNext();
}

void NotebookNavigationDlg::OnHotKeyPrev(wxKeyEvent& event)
{
    wxUnusedVar(event);
    SelectPrev();
}

void NotebookNavigationDlg::SelectNext()
{
    // Navigate Down
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    if (item.IsOk()) {
        int row = m_dvListCtrl->ItemToRow(item);
        if (row < (int)(m_dvListCtrl->GetItemCount() - 1)) {
            ++row;
            item = m_dvListCtrl->RowToItem(row);
            m_dvListCtrl->Select(item);
            m_dvListCtrl->EnsureVisible(item);
        } else {
            // Select the last item
            item = m_dvListCtrl->RowToItem(0);
            m_dvListCtrl->Select(item);
            m_dvListCtrl->EnsureVisible(item);
        }
    }
}

void NotebookNavigationDlg::SelectPrev()
{
    // Navigate Up
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    if (item.IsOk()) {
        int row = m_dvListCtrl->ItemToRow(item);
        if (row > 0) {
            --row;
            item = m_dvListCtrl->RowToItem(row);
            m_dvListCtrl->Select(item);
            m_dvListCtrl->EnsureVisible(item);

        } else {
            // Select the last item
            row = m_dvListCtrl->GetItemCount() - 1;
            item = m_dvListCtrl->RowToItem(row);
            m_dvListCtrl->Select(item);
            m_dvListCtrl->EnsureVisible(item);
        }
    }
}