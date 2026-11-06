#include "NotebookNavigationDlg.h"

#include "WindowUtils.hpp"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"

#include <wx/app.h>
#include <wx/dynarray.h>

wxDEFINE_EVENT(wxEVT_BOOKNAV_SELECTED, wxBookCtrlEvent);

NotebookNavigationDlg::NotebookNavigationDlg(wxWindow* parent, clTabHistory::Ptr_t history)
    : wxPopupTransientWindow(parent, wxBORDER_DEFAULT)
    , m_ctrlKeyTimer{this}
    , m_history{history}
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    m_dvListCtrl =
        new wxDataViewListCtrl(this,
                               wxID_ANY,
                               wxDefaultPosition,
                               wxDefaultSize,
                               wxDV_NO_HEADER | wxDV_ROW_LINES | wxDV_SINGLE | wxWANTS_CHARS | wxBORDER_NONE);
    sizer->Add(m_dvListCtrl, wxSizerFlags(1).Expand().Border(wxALL, FromDIP(5)));
    m_dvListCtrl->AppendBitmapColumn(
        _("Icon"), m_dvListCtrl->GetColumnCount(), wxDATAVIEW_CELL_INERT, FromDIP(30), wxALIGN_CENTER);
    m_dvListCtrl->AppendTextColumn(_("Text"), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);

    ::AdjustDataViewAlternateColour(m_dvListCtrl);
    Initialise();
    FinalizeCtor();
}

NotebookNavigationDlg::~NotebookNavigationDlg()
{
    clDEBUG() << "NotebookNavigationDlg DTor is called" << endl;
    m_ctrlKeyTimer.Stop();
    Unbind(wxEVT_TIMER, &NotebookNavigationDlg::OnTimer, this);
#if defined(__WXMSW__) || defined(__WXMAC__)
    UnregisterHotKey(XRCID("tab-next-entry"));
    UnregisterHotKey(XRCID("tab-prev-entry"));
    wxTheApp->Unbind(wxEVT_HOTKEY, &NotebookNavigationDlg::OnHotKeyNext, this, XRCID("tab-next-entry"));
    wxTheApp->Unbind(wxEVT_HOTKEY, &NotebookNavigationDlg::OnHotKeyPrev, this, XRCID("tab-prev-entry"));
#endif
    m_dvListCtrl->Unbind(wxEVT_KEY_DOWN, &NotebookNavigationDlg::OnKeyDown, this);
    m_dvListCtrl->Unbind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &NotebookNavigationDlg::OnItemActivated, this);
    m_dvListCtrl->Unbind(wxEVT_KEY_UP, &NotebookNavigationDlg::OnKeyUp, this);

    for (int row = 0; row < m_dvListCtrl->GetItemCount(); ++row) {
        TabData* d = (TabData*)m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(row));
        wxDELETE(d);
    }
    m_dvListCtrl->DeleteAllItems();
}

void NotebookNavigationDlg::NotifyAndClose()
{
    if (m_dismissed) {
        return;
    }
    m_dismissed = true;

    wxDataViewItem selectedItem = m_dvListCtrl->GetSelection();
    int selection{wxNOT_FOUND};
    if (selectedItem.IsOk()) {
        TabData* d = reinterpret_cast<TabData*>(m_dvListCtrl->GetItemData(selectedItem));
        selection = d->index;
    }

    // Hide the popup first, then switch the tab. Dismiss() alone does not call OnDismiss()
    DismissAndNotify();

    if (selection != wxNOT_FOUND && selection < static_cast<int>(clGetManager()->GetMainNotebook()->GetPageCount())) {
        wxBookCtrlEvent event{wxEVT_BOOKNAV_SELECTED};
        event.SetSelection(selection);
        EventNotifier::Get()->ProcessEvent(event);
    }
}

void NotebookNavigationDlg::OnDismiss()
{
    // Called after the popup is hidden: by DismissAndNotify(), or by a click outside of it
    m_dismissed = true;
    m_ctrlKeyTimer.Stop();
    CallAfter([this]() {
        clDEBUG() << "Destroy is called" << endl;
        Destroy();
    });
}

void NotebookNavigationDlg::ShowPopup()
{
    wxWindow* parent = GetParent();
    const wxSize size = FromDIP(wxSize(500, 300));
    SetSize(size);
    wxPoint parentPos = parent->ClientToScreen(wxPoint(0, 0));
    wxSize parentSize = parent->GetClientSize();
    Position(parentPos + wxPoint((parentSize.x - size.x) / 2, (parentSize.y - size.y) / 2), wxSize(0, 0));
    Popup(m_dvListCtrl);
    StylePopupWindow(this, 10.0);

    // The popup does not get the key-up of the Ctrl key: poll for it
    m_ctrlKeyTimer.Start(30);
}

void NotebookNavigationDlg::OnTimer(wxTimerEvent& event)
{
    wxUnusedVar(event);
    if (!wxGetKeyState(WXK_RAW_CONTROL)) {
        NotifyAndClose();
    }
}

void NotebookNavigationDlg::FinalizeCtor()
{
    if (m_dvListCtrl->GetItemCount() > 1) {
        m_dvListCtrl->Select(m_dvListCtrl->RowToItem(1));
    } else if (m_dvListCtrl->GetItemCount() == 1) {
        m_dvListCtrl->Select(m_dvListCtrl->RowToItem(0));
    }

    SetSize(FromDIP(wxSize(500, 300)));

#if defined(__WXMSW__) || defined(__WXMAC__)
    RegisterHotKey(XRCID("tab-next-entry"), wxMOD_RAW_CONTROL, WXK_TAB);
    RegisterHotKey(XRCID("tab-prev-entry"), wxMOD_RAW_CONTROL | wxMOD_SHIFT, WXK_TAB);
    wxTheApp->Bind(wxEVT_HOTKEY, &NotebookNavigationDlg::OnHotKeyNext, this, XRCID("tab-next-entry"));
    wxTheApp->Bind(wxEVT_HOTKEY, &NotebookNavigationDlg::OnHotKeyPrev, this, XRCID("tab-prev-entry"));
#endif
    m_dvListCtrl->Bind(wxEVT_KEY_DOWN, &NotebookNavigationDlg::OnKeyDown, this);
    m_dvListCtrl->Bind(wxEVT_KEY_UP, &NotebookNavigationDlg::OnKeyUp, this);
    m_dvListCtrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &NotebookNavigationDlg::OnItemActivated, this);
    Bind(wxEVT_TIMER, &NotebookNavigationDlg::OnTimer, this);
}

void NotebookNavigationDlg::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_ESCAPE) {
        m_dismissed = true;
        DismissAndNotify();
        return;
    }

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
        NotifyAndClose();
    } else {
        event.Skip();
    }
}

void NotebookNavigationDlg::OnItemActivated(wxDataViewEvent& event)
{
    event.Skip();
    LOG_IF_TRACE { clDEBUG1() << "NotebookNavigationDlg::OnItemActivated" << endl; }
    NotifyAndClose();
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
        int itemCount = m_dvListCtrl->GetItemCount();
        if (row < (itemCount - 1)) {
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

void NotebookNavigationDlg::Initialise()
{
    clTab::Vec_t allTabs;
    clGetManager()->GetAllTabs(allTabs);
    std::map<void*, clTab> tabsInfoMap;
    for (size_t i = 0; i < allTabs.size(); ++i) {
        tabsInfoMap.insert(std::make_pair((void*)allTabs.at(i).window, allTabs.at(i)));
    }
    const std::vector<wxWindow*>& windows = m_history->GetHistory();
    // Populate the list
    auto* book = clGetManager()->GetMainNotebook();
    for (size_t i = 0; i < windows.size(); ++i) {
        int index = book->GetPageIndex(windows[i]);
        if (index == wxNOT_FOUND)
            continue;
        wxString label = book->GetPageText(index);
        wxBitmap bmp = book->GetPageBitmap(index);

        wxVector<wxVariant> cols;
        TabData* d = new TabData;
        d->bmp = bmp;
        d->label = label;
        d->index = index;

        // add extra info
        std::map<void*, clTab>::iterator iter = tabsInfoMap.find(windows[i]);
        bool isModified = false;
        if (iter != tabsInfoMap.end()) {
            d->isFile = iter->second.isFile;
            d->filename = iter->second.filename;
            isModified = iter->second.isModified;
        }

        // Prepare the display item
        wxString text;
        if (d->isFile && d->filename.GetDirCount()) {
            wxFileName fn(d->filename.GetFullName());
            fn.AppendDir(d->filename.GetDirs().Last());
            text << fn.GetFullPath();
        } else {
            text << d->label;
        }

        // A tab that is not a file (e.g. the diff view) shows its own bitmap.
        // For a file, try to match one by file name.
        // If no match is found, fall back to the "placeholder" icon.
        wxBitmap bmpForFile = clGetManager()->GetStdIcons()->LoadBitmap("placeholder");
        wxString fullname = d->filename.GetFullName();
        if (!d->isFile && d->bmp.IsOk()) {
            bmpForFile = d->bmp;
        } else if (!fullname.empty() && FileExtManager::GetType(fullname) != FileExtManager::TypeOther) {
            bmpForFile = clGetManager()->GetStdIcons()->GetBitmapForFile(d->filename.GetFullName(), false);
        }

        // If the tab is modified, prepend the "disk save" indicator to the text
        if (isModified) {
            text.Prepend(L"\U0001F4BE ");
        }

        // Column 0 ("Icon") is a bitmap column
        wxVariant colIcon;
        colIcon << wxBitmapBundle(bmpForFile);
        // Column 1 ("Text") is a plain text column
        wxVariant colPath = text;

        cols.push_back(colIcon);
        cols.push_back(colPath);
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)d);
    }
}
