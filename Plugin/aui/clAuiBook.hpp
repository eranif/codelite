#pragma once

#include "JSON.h"
#include "clTabHistory.h"
#include "clTabRenderer.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <wx/aui/auibook.h>
#include <wx/colour.h>

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif

class WXDLLIMPEXP_SDK clAuiBook : public wxAuiNotebook
{
public:
    clAuiBook(wxWindow* parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0);
    ~clAuiBook() override;

    /// Fills the provided vector with info for all current tabs
    size_t GetAllTabs(clTabInfo::Vec_t& tabs);

    /// Returns the tab history object
    clTabHistory::Ptr_t GetHistory() const { return m_history; }

    /// Moves the currently active page to the specified index
    void MoveActivePage(int newIndex);

    int SetSelection(size_t newPage) override;
    int ChangeSelection(size_t n) override;
    wxBorder GetDefaultBorder() const override;

    /// Enable or disable events fired from the book
    void EnableEvents(bool b);

    bool AreEventsEnabled() const { return m_eventsEnabled; }

    /// Removes pages from history that are no longer present in the book
    void UpdateHistory();

    /// Sets whether the tabs are allowed to have close buttons
    void SetCanHaveCloseButton(bool b) { m_canHaveCloseButton = b; }

    int GetPageIndex(const wxString& name) const;
    int GetPageIndex(wxWindow* win) const;

    /// Deletes a page. If notify is true, triggers closing/closed events.
    bool DeletePage(size_t index, bool notify = true);

    /// Removes a page. If notify is true, triggers closing/closed events.
    bool RemovePage(size_t index, bool notify = true);

    int GetPageBitmapIndex(size_t n) const;
    void SetTabDirection(wxDirection d);

    /// Applies the CodeLite tab art to the specified AuiNotebook.
    /// @param book The notebook to update
    /// @param outlineColour The colour of the outline of the current tab. If not valid, the colour is calculated from
    /// the background colour of the editor. Used with wxWidgets 3.3 and later only
    static void ApplyTabArt(wxAuiNotebook* book, const wxColour& outlineColour = wxNullColour);

protected:
    void OnPageClosed(wxAuiNotebookEvent& event);
    void OnPageClosing(wxAuiNotebookEvent& event);
    void OnPageChanged(wxAuiNotebookEvent& event);
    void OnPageChanging(wxAuiNotebookEvent& event);
    void OnPageRightDown(wxAuiNotebookEvent& event);
    void OnTabAreaDoubleClick(wxAuiNotebookEvent& event);
    void OnPageDoubleClick(wxAuiNotebookEvent& event);
    void OnPreferences(clCommandEvent& event);
    void OnEditorConfig(wxCommandEvent& event);

    void UpdatePreferences();
    /// Call UpdatePreferences() once, after the pending events. The colours and the editor settings usually change
    /// together, and updating the art is expensive
    void ScheduleUpdatePreferences();
    void SetBookArt();

private:
    clTabHistory::Ptr_t m_history;
    bool m_eventsEnabled{true};
    bool m_canHaveCloseButton{false};
    bool m_updatePreferencesPending{false};
};

class WXDLLIMPEXP_SDK clAuiBookEventsDisabler
{
public:
    clAuiBookEventsDisabler(clAuiBook* book)
        : m_book(book)
    {
        if (m_book) {
            m_oldState = m_book->AreEventsEnabled();
            m_book->EnableEvents(false);
        }
    }
    ~clAuiBookEventsDisabler()
    {
        if (m_book) {
            m_book->EnableEvents(m_oldState);
        }
    }

private:
    clAuiBook* m_book = nullptr;
    bool m_oldState = true;
};

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
