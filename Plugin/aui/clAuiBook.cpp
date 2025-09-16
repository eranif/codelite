#include "clAuiBook.hpp"

#include "Notebook.h" // For the book events.
#include "clSystemSettings.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "globals.h"

#include <vector>
#include <wx/aui/tabart.h>
#include <wx/wupdlock.h>

#ifdef __WXGTK__
static constexpr size_t X_SPACER = 15;
#else
static constexpr size_t X_SPACER = 10;
#endif

// Configurable via Settings -> Preferences -> Tabs
namespace
{
int NotebookFlagsToAuiBookStyle(long style_flags, NotebookStyle style, wxAuiNotebookOption option)
{
    if (style_flags & style) {
        return option;
    } else {
        return 0;
    }
}
} // namespace

static size_t Y_SPACER = 5;

clAuiBook::clAuiBook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    long aui_book_style = 0;
    aui_book_style |= NotebookFlagsToAuiBookStyle(style, kNotebook_BottomTabs, wxAUI_NB_BOTTOM);
    aui_book_style |= NotebookFlagsToAuiBookStyle(style, kNotebook_ShowFileListButton, wxAUI_NB_WINDOWLIST_BUTTON);
    aui_book_style |=
        NotebookFlagsToAuiBookStyle(style, kNotebook_CloseButtonOnActiveTab, wxAUI_NB_CLOSE_ON_ACTIVE_TAB);
    aui_book_style |=
        NotebookFlagsToAuiBookStyle(style, kNotebook_MouseMiddleClickClosesTab, wxAUI_NB_MIDDLE_CLICK_CLOSE);
    aui_book_style |= NotebookFlagsToAuiBookStyle(style, kNotebook_FixedWidth, wxAUI_NB_TAB_FIXED_WIDTH);
    aui_book_style |= NotebookFlagsToAuiBookStyle(style, kNotebook_AllowDnD, wxAUI_NB_TAB_MOVE);
    if ((aui_book_style & wxAUI_NB_BOTTOM) == 0) {
        aui_book_style |= wxAUI_NB_TOP;
    }
    wxAuiNotebook::Create(parent, id, pos, size, aui_book_style);
    SetBookArt();

    wxFont font = clTabRenderer::GetTabFont(false);
    SetFont(font);
    SetMeasuringFont(font);
    SetNormalFont(font);
    SetSelectedFont(font);

    m_history.reset(new clTabHistory());
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGING, &clAuiBook::OnPageChanging, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &clAuiBook::OnPageChanged, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &clAuiBook::OnPageClosing, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &clAuiBook::OnPageClosed, this);
    Bind(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, &clAuiBook::OnPageRightDown, this);
    Bind(wxEVT_AUINOTEBOOK_BG_DCLICK, &clAuiBook::OnTabAreaDoubleClick, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_SETTINGS_CHANGED, &clAuiBook::OnPreferences, this);
    CallAfter(&clAuiBook::UpdatePreferences);
}

clAuiBook::~clAuiBook()
{
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CHANGING, &clAuiBook::OnPageChanging, this);
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &clAuiBook::OnPageChanged, this);
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &clAuiBook::OnPageClosing, this);
    Unbind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &clAuiBook::OnPageClosed, this);
    Unbind(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, &clAuiBook::OnPageRightDown, this);
    Unbind(wxEVT_AUINOTEBOOK_BG_DCLICK, &clAuiBook::OnTabAreaDoubleClick, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_SETTINGS_CHANGED, &clAuiBook::OnPreferences, this);
}

size_t clAuiBook::GetAllTabs(clTabInfo::Vec_t& tabs)
{
    size_t count = GetPageCount();
    tabs.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        clTabInfo::Ptr_t info(new clTabInfo(nullptr, 0, GetPage(i), GetPageText(i), wxNOT_FOUND));
        tabs.emplace_back(info);
    }
    return tabs.size();
}

void clAuiBook::MoveActivePage(int newIndex)
{
    // freeze the UI
    wxWindowUpdateLocker locker{this};

    // no events while we are doing the tab movement
    clAuiBookEventsDisabler disabler{this};

    int cursel = GetSelection();
    CHECK_COND_RET(cursel != wxNOT_FOUND);

    wxWindow* page = GetCurrentPage();
    CHECK_PTR_RET(page);

    wxString label = GetPageText(cursel);
    wxBitmap bmp = GetPageBitmap(cursel);
    if (RemovePage(cursel, false)) {
        InsertPage(newIndex, page, label, true, bmp);
    }
}

int clAuiBook::SetSelection(size_t newPage)
{
    size_t res = wxAuiNotebook::SetSelection(newPage);
    m_history->Push(GetCurrentPage());
    return res;
}

int clAuiBook::ChangeSelection(size_t n)
{
    size_t res = wxAuiNotebook::ChangeSelection(n);
    m_history->Push(GetCurrentPage());
    return res;
}

wxBorder clAuiBook::GetDefaultBorder() const { return wxBORDER_NONE; }

void clAuiBook::OnPageClosed(wxAuiNotebookEvent& event)
{
    event.Skip();
    if (!m_eventsEnabled) {
        return;
    }
    // Ensure that the history contains only existing tabs
    std::vector<wxWindow*> windows;
    size_t count = GetPageCount();
    windows.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        windows.emplace_back(GetPage(i));
    }
    m_history->Compact(windows, false);

    wxBookCtrlEvent event_closed(wxEVT_BOOK_PAGE_CLOSED);
    event_closed.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(event_closed);
}

void clAuiBook::OnPageChanged(wxAuiNotebookEvent& event)
{
    wxUnusedVar(event);
    if (!m_eventsEnabled) {
        return;
    }

    // Get the new selection and move it to the top of the hisotry list
    wxWindow* win = GetCurrentPage();
    CHECK_PTR_RET(win);

    m_history->Pop(win);
    m_history->Push(win);

    // Send an event
    wxBookCtrlEvent changed_event(wxEVT_BOOK_PAGE_CHANGED);
    changed_event.SetEventObject(this);
    changed_event.SetSelection(GetSelection());
    GetEventHandler()->AddPendingEvent(changed_event);
}

void clAuiBook::OnPageChanging(wxAuiNotebookEvent& event)
{
    if (!m_eventsEnabled) {
        // Allow changing
        event.Skip();
        return;
    }

    // wrap this event with our own event
    wxBookCtrlEvent event_changing(wxEVT_BOOK_PAGE_CHANGING);
    event_changing.SetEventObject(this);
    event_changing.SetSelection(event.GetSelection());
    event_changing.SetOldSelection(GetSelection());
    GetEventHandler()->ProcessEvent(event_changing);
    if (!event_changing.IsAllowed()) {
        // Vetoed by the user, veto the original event and return
        event.Veto();
        return;
    }
    // Allow changing
    event.Skip();
}

void clAuiBook::OnPageDoubleClick(wxAuiNotebookEvent& event)
{
    if (!m_eventsEnabled) {
        // Allow changing
        event.Skip();
        return;
    }
    wxUnusedVar(event);
    wxBookCtrlEvent e(wxEVT_BOOK_TAB_DCLICKED);
    e.SetEventObject(this);
    e.SetSelection(GetSelection());
    GetEventHandler()->AddPendingEvent(e);
}

void clAuiBook::OnTabAreaDoubleClick(wxAuiNotebookEvent& event)
{
    if (!m_eventsEnabled) {
        // Allow changing
        event.Skip();
        return;
    }

    wxBookCtrlEvent e(wxEVT_BOOK_NEW_PAGE);
    e.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(e);
}

void clAuiBook::OnPageRightDown(wxAuiNotebookEvent& event)
{
    if (!m_eventsEnabled) {
        // Allow changing
        event.Skip();
        return;
    }

    wxBookCtrlEvent menuEvent(wxEVT_BOOK_TAB_CONTEXT_MENU);
    menuEvent.SetEventObject(this);
    menuEvent.SetSelection(GetSelection());
    GetEventHandler()->ProcessEvent(menuEvent);
}

void clAuiBook::OnPageClosing(wxAuiNotebookEvent& event)
{
    if (!m_eventsEnabled) {
        // Allow changing
        event.Skip();
        return;
    }

    wxBookCtrlEvent eventClosing(wxEVT_BOOK_PAGE_CLOSING);
    eventClosing.SetEventObject(this);
    eventClosing.SetSelection(GetSelection());
    GetEventHandler()->ProcessEvent(eventClosing);
    if (!eventClosing.IsAllowed()) {
        // Vetoed
        event.Veto();
        return;
    }
    event.Skip();
}

void clAuiBook::UpdatePreferences()
{
    auto style = GetWindowStyle();
    clSYSTEM() << "Updating editor preferences: (" << m_canHaveCloseButton << ")" << (wxUIntPtr)this
               << ", MainNotebook:" << (wxUIntPtr)clGetManager()->GetMainNotebook() << endl;
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    bool show_x_on_tab = options->IsTabHasXButton();

    style &= ~wxAUI_NB_CLOSE_ON_ALL_TABS;
    if (m_canHaveCloseButton) {
        if (show_x_on_tab) {
            style |= wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
        } else {
            style &= ~wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
        }
    } else {
        style &= ~wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
    }

    // update the tab height
    int tabHeight = options->GetNotebookTabHeight();
    switch (tabHeight) {
    case OptionsConfig::nbTabHt_Tiny:
        Y_SPACER = tabHeight + 2;
        break;
    case OptionsConfig::nbTabHt_Short:
        Y_SPACER = tabHeight + 2;
        break;
    default:
    case OptionsConfig::nbTabHt_Medium:
        Y_SPACER = tabHeight + 2;
        break;
    case OptionsConfig::nbTabHt_Tall:
        Y_SPACER = tabHeight + 2;
        break;
    }

    // update the art
    SetBookArt();
    SetWindowStyle(style);
    Refresh();
}

void clAuiBook::SetBookArt()
{
    wxFont font = clTabRenderer::GetTabFont(false);

    auto art = new wxAuiDefaultTabArt();
    art->SetMeasuringFont(font);
    art->SetNormalFont(font);
    art->SetSelectedFont(font);
    SetArtProvider(art);
}

void clAuiBook::OnPreferences(wxCommandEvent& event)
{
    event.Skip();
    UpdatePreferences();
}

void clAuiBook::EnableEvents(bool b) { m_eventsEnabled = b; }

void clAuiBook::UpdateHistory()
{
    // Ensure that the history contains only existing tabs
    std::vector<wxWindow*> windows;
    size_t count = GetPageCount();
    windows.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        windows.emplace_back(GetPage(i));
    }
    m_history->Compact(windows, true);
}

int clAuiBook::GetPageIndex(wxWindow* win) const { return FindPage(win); }

int clAuiBook::GetPageIndex(const wxString& name) const
{
    for (size_t i = 0; i < GetPageCount(); ++i) {
        if (GetPageText(i) == name) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

bool clAuiBook::DeletePage(size_t index, bool notify)
{
    if (notify) {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSING);
        event.SetEventObject(this);
        event.SetSelection(index);
        GetEventHandler()->ProcessEvent(event);
        if (!event.IsAllowed()) {
            // Vetoed
            return false;
        }
    }

    if (!wxAuiNotebook::DeletePage(index)) {
        return false;
    }

    if (notify) {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSED);
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }
    return true;
}

bool clAuiBook::RemovePage(size_t index, bool notify)
{
    if (notify) {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSING);
        event.SetEventObject(this);
        event.SetSelection(index);
        GetEventHandler()->ProcessEvent(event);
        if (!event.IsAllowed()) {
            // Vetoed
            return false;
        }
    }

    if (!wxAuiNotebook::RemovePage(index)) {
        return false;
    }

    if (notify) {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSED);
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }
    return true;
}

int clAuiBook::GetPageBitmapIndex([[maybe_unused]] size_t n) const
{
    // Stub method
    return wxNOT_FOUND;
}

void clAuiBook::SetTabDirection([[maybe_unused]] wxDirection d)
{
    // Stub method
}
