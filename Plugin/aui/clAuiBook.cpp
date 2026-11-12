#include "clAuiBook.hpp"

#include "ColoursAndFontsManager.h"
#include "Notebook.h" // For the book events.
#include "aui/clAuiFlatTabArt.hpp"
#include "editor_config.h"
#include "event_notifier.h"
#include "globals.h"

#if wxCHECK_VERSION(3, 3, 0)
#include "GlyphTabArt.h"
#endif

#include <memory>
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

clAuiBook::clAuiBook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    long aui_book_style = 0;
    aui_book_style |= NotebookFlagsToAuiBookStyle(style, kNotebook_BottomTabs, wxAUI_NB_BOTTOM);
    aui_book_style |= NotebookFlagsToAuiBookStyle(style, kNotebook_ShowFileListButton, wxAUI_NB_WINDOWLIST_BUTTON);
    aui_book_style |= NotebookFlagsToAuiBookStyle(
        style,
        static_cast<NotebookStyle>(kNotebook_ShowFileListButton | kNotebook_ShowScrollButtons),
        wxAUI_NB_SCROLL_BUTTONS);
    aui_book_style |=
        NotebookFlagsToAuiBookStyle(style, kNotebook_CloseButtonOnActiveTab, wxAUI_NB_CLOSE_ON_ACTIVE_TAB);
    aui_book_style |=
        NotebookFlagsToAuiBookStyle(style, kNotebook_MouseMiddleClickClosesTab, wxAUI_NB_MIDDLE_CLICK_CLOSE);
    aui_book_style |= NotebookFlagsToAuiBookStyle(style, kNotebook_FixedWidth, wxAUI_NB_TAB_FIXED_WIDTH);
    aui_book_style |= NotebookFlagsToAuiBookStyle(style, kNotebook_AllowDnD, wxAUI_NB_TAB_MOVE);
    aui_book_style |= NotebookFlagsToAuiBookStyle(style, kNotebook_AllowDnD, wxAUI_NB_TAB_SPLIT);
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

    m_history = std::make_shared<clTabHistory>();
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGING, &clAuiBook::OnPageChanging, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &clAuiBook::OnPageChanged, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &clAuiBook::OnPageClosing, this);
    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSED, &clAuiBook::OnPageClosed, this);
    Bind(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, &clAuiBook::OnPageRightDown, this);
    Bind(wxEVT_AUINOTEBOOK_BG_DCLICK, &clAuiBook::OnTabAreaDoubleClick, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &clAuiBook::OnPreferences, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &clAuiBook::OnEditorConfig, this);
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
    EventNotifier::Get()->Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &clAuiBook::OnPreferences, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &clAuiBook::OnEditorConfig, this);
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
    eventClosing.SetSelection(event.GetSelection());
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
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    bool show_x_on_tab = options->IsTabHasXButton();

    style &= ~(wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB);
    if (m_canHaveCloseButton) {
        if (show_x_on_tab) {
            style |= wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
        } else {
            style &= ~wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
        }
    }

    // update the tab height: the tab-height preference is read directly by
    // clAuiFlatTabArt::GetPageTabSize(). Recreating the art provider below
    // (via SetBookArt) updates the per-tab drawing, and UpdateTabCtrlHeight()
    // re-measures the tab strip itself so its background grows with the tabs.

    // update the art
    SetWindowStyle(style);
    SetBookArt();
    Refresh();
}

#if wxCHECK_VERSION(3, 3, 0)
namespace
{
/// GlyphTabArt that follows the colours of the editor: when the theme or the settings of the editor change, it gives
/// its notebook a new art (a GlyphTabArt does not follow the system colours). Only the art that is owned by the
/// notebook listens to the events: the copies that wxAuiNotebook gives to its tab controls (Clone()) are plain
/// GlyphTabArt objects.
class clGlyphTabArt : public GlyphTabArt
{
public:
    /// Create an art for `book`, or return nullptr if the colours of the editor are not available.
    /// `outlineColour`: the outline of the current tab, or an invalid colour for the default one
    /// `followTheme`: when true, the art replaces itself when the theme or the editor settings change. Use false if the
    /// owner of the notebook updates the art by itself (otherwise the two updates may overwrite each other)
    static clGlyphTabArt* Create(wxAuiNotebook* book, const wxColour& outlineColour, bool followTheme)
    {
        auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
        if (lexer == nullptr) {
            return nullptr;
        }
        auto theme = lexer->AsTerminalTheme();
        auto* art = new clGlyphTabArt(book, theme, outlineColour, followTheme);
        art->SetOutlineColour(outlineColour.IsOk()
                                  ? outlineColour
                                  : (lexer->IsDark() ? theme.bg.ChangeLightness(120) : theme.bg.ChangeLightness(80)));

        // The "Tab height" setting is the space above and below the label of a tab. As clAuiFlatTabArt, every step of
        // the setting adds 1 DIP at each side. The default setting (Short) gives the default padding of GlyphTabArt: 6
        // DIPs, and 10 on GTK
#ifdef __WXGTK__
        constexpr int kPaddingBase = 8;
#else
        constexpr int kPaddingBase = 4;
#endif
        art->SetPaddingY(EditorConfigST::Get()->GetOptions()->GetNotebookTabHeight() + kPaddingBase);
        return art;
    }

    /// The destructor runs inside SetArtProvider() of the notebook (it deletes the previous art), also when the art is
    /// replaced twice in a row: the events are unbound there, so a deleted art never gets an event
    ~clGlyphTabArt() override
    {
        EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clGlyphTabArt::OnSysColoursChanged, this);
        EventNotifier::Get()->Unbind(wxEVT_EDITOR_SETTINGS_CHANGED, &clGlyphTabArt::OnEditorSettingsChanged, this);
    }

private:
    clGlyphTabArt(wxAuiNotebook* book, const wxTerminalTheme& theme, const wxColour& outlineColour, bool followTheme)
        : GlyphTabArt(theme, Style::Browser)
        , m_book{book}
        , m_outlineColour{outlineColour}
    {
        if (followTheme) {
            EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clGlyphTabArt::OnSysColoursChanged, this);
            EventNotifier::Get()->Bind(wxEVT_EDITOR_SETTINGS_CHANGED, &clGlyphTabArt::OnEditorSettingsChanged, this);
        }
    }

    void OnSysColoursChanged(clCommandEvent& event)
    {
        event.Skip();
        ReplaceArt();
    }

    void OnEditorSettingsChanged(wxCommandEvent& event)
    {
        event.Skip();
        ReplaceArt();
    }

    /// Give the notebook a new art. This art is deleted by SetArtProvider(), so it is done after the event handler
    /// returns (the lambda does not use `this`). The editor settings and the system colours usually change together:
    /// queue only one update
    void ReplaceArt()
    {
        if (m_updatePending) {
            return;
        }
        m_updatePending = true;
        wxAuiNotebook* book = m_book;
        const wxColour outlineColour = m_outlineColour;
        book->CallAfter([book, outlineColour]() {
            clAuiBook::ApplyTabArt(book, outlineColour, true);
            book->Refresh();
            book->SendSizeEvent();
        });
    }

    wxAuiNotebook* m_book{nullptr};
    wxColour m_outlineColour;
    bool m_updatePending{false}; // The art is deleted when the update is done, so the flag is never cleared
};
} // namespace
#endif // wxCHECK_VERSION(3, 3, 0)

void clAuiBook::ApplyTabArt(wxAuiNotebook* book, const wxColour& outlineColour, bool followTheme)
{
#if wxCHECK_VERSION(3, 3, 0)
    if (auto* art = clGlyphTabArt::Create(book, outlineColour, followTheme)) {
        book->SetArtProvider(art);
        return;
    }
#endif

    wxFont font = clTabRenderer::GetTabFont(false);
    auto art = new clAuiFlatTabArt();
    art->SetMeasuringFont(font);
    art->SetNormalFont(font);
    art->SetSelectedFont(font);
    book->SetArtProvider(art);
}

void clAuiBook::SetBookArt() { ApplyTabArt(this); }

void clAuiBook::ScheduleUpdatePreferences()
{
    if (m_updatePreferencesPending) {
        return;
    }
    m_updatePreferencesPending = true;
    CallAfter([this]() {
        m_updatePreferencesPending = false;
        UpdatePreferences();
    });
}

void clAuiBook::OnEditorConfig(wxCommandEvent& event)
{
    event.Skip();
    ScheduleUpdatePreferences();
}

void clAuiBook::OnPreferences(clCommandEvent& event)
{
    event.Skip();
    ScheduleUpdatePreferences();
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
