#include "NotebookNavigationDlg.h"

#include "WindowUtils.hpp"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"

#include <wx/app.h>
#include <wx/dynarray.h>
#include <wx/event.h>

wxDEFINE_EVENT(wxEVT_BOOKNAV_SELECTED, wxBookCtrlEvent);

/// Catches the keys that go to the popup (or to its children) while it is shown
class TabSwitcherEventFilter : public wxEventFilter
{
public:
    explicit TabSwitcherEventFilter(NotebookNavigationDlg* parent)
        : m_navDlg{parent}
    {
        wxEvtHandler::AddFilter(this);
    }

    ~TabSwitcherEventFilter() override { wxEvtHandler::RemoveFilter(this); }

    int FilterEvent(wxEvent& event) override
    {
        const auto type = event.GetEventType();
        if (type != wxEVT_CHAR_HOOK && type != wxEVT_KEY_UP) {
            return Event_Skip;
        }

        // The event object is the window that has the focus. With the generic wxDataViewCtrl,
        // it is a child of the list control
        auto* win = wxDynamicCast(event.GetEventObject(), wxWindow);
        if (win == nullptr || (win != m_navDlg && !m_navDlg->IsDescendant(win))) {
            return Event_Skip;
        }

        const auto& keyEvent = static_cast<const wxKeyEvent&>(event);
        if (type == wxEVT_CHAR_HOOK) {
            if (keyEvent.GetKeyCode() == WXK_ESCAPE) {
                m_navDlg->CallAfter(&NotebookNavigationDlg::Cancel);
                return Event_Processed;
            }
            if (keyEvent.GetKeyCode() == WXK_TAB && keyEvent.RawControlDown()) {
                if (keyEvent.ShiftDown()) {
                    m_navDlg->CallAfter(&NotebookNavigationDlg::SelectPrev);
                } else {
                    m_navDlg->CallAfter(&NotebookNavigationDlg::SelectNext);
                }
                return Event_Processed;
            }
        } else if (keyEvent.GetKeyCode() == WXK_RAW_CONTROL) {
            // Ctrl was released while the popup (or one of its children) has the focus: switch to the selected tab.
            // Let other handlers see the event as well
            m_navDlg->CallAfter(&NotebookNavigationDlg::NotifyAndClose);
        }
        return Event_Skip;
    }

private:
    NotebookNavigationDlg* m_navDlg{nullptr};
};

NotebookNavigationDlg::NotebookNavigationDlg(wxWindow* parent, clTabHistory::Ptr_t history)
    : wxPopupTransientWindow(parent, wxBORDER_DEFAULT)
    , m_ctrlKeyTimer{this}
    , m_history{history}
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    m_dvListCtrl = new wxDataViewListCtrl(
        this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_NO_HEADER | wxDV_SINGLE | wxWANTS_CHARS | wxBORDER_NONE);
    sizer->Add(m_dvListCtrl, wxSizerFlags(1).Expand().Border(wxALL, FromDIP(5)));
    m_dvListCtrl->AppendBitmapColumn(
        _("Icon"), m_dvListCtrl->GetColumnCount(), wxDATAVIEW_CELL_INERT, FromDIP(30), wxALIGN_CENTER);
    m_dvListCtrl->AppendTextColumn(_("Text"), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);

    Initialise();
    FinalizeCtor();
#ifdef __WXGTK__
    // GTK: the popup can be made transparent only before it is shown
    StylePopupWindow(this);
#endif
    m_eventFilter = std::make_unique<TabSwitcherEventFilter>(this);
}

NotebookNavigationDlg::~NotebookNavigationDlg()
{
    m_eventFilter.reset();

    clDEBUG() << "NotebookNavigationDlg DTor is called" << endl;
    m_ctrlKeyTimer.Stop();
    Unbind(wxEVT_TIMER, &NotebookNavigationDlg::OnTimer, this);
    m_dvListCtrl->Unbind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &NotebookNavigationDlg::OnItemActivated, this);

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
    const wxWindow* parent = GetParent();

    // At the top of the application's client area, centred horizontally,
    // 1/2 of its width and 1/4 of its height (screen coordinates).
    const wxRect parentRect(parent->ClientToScreen(wxPoint(0, 0)), parent->GetClientSize());
    wxRect ourRect(0, 0, parentRect.GetWidth() / 2, parentRect.GetHeight() / 4);
    ourRect = ourRect.CenterIn(parentRect, wxHORIZONTAL);
    ourRect.SetY(parentRect.GetY());
    SetSize(ourRect);

    Popup(m_dvListCtrl);
#ifndef __WXGTK__
    StylePopupWindow(this);
#endif

    // The popup does not always get the key-up of the Ctrl key (the focus may be elsewhere): poll for it. The key-up
    // handler of the event filter reacts faster when the focus is in the popup. Both end in NotifyAndClose(), which
    // runs only once
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
    m_dvListCtrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &NotebookNavigationDlg::OnItemActivated, this);
    Bind(wxEVT_TIMER, &NotebookNavigationDlg::OnTimer, this);
}

void NotebookNavigationDlg::Cancel()
{
    if (m_dismissed) {
        return;
    }
    m_dismissed = true;
    DismissAndNotify();
}

void NotebookNavigationDlg::OnItemActivated(wxDataViewEvent& event)
{
    event.Skip();
    LOG_IF_TRACE { clDEBUG1() << "NotebookNavigationDlg::OnItemActivated" << endl; }
    NotifyAndClose();
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
