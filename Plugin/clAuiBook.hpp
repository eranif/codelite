#ifndef CLAUIBOOK_HPP
#define CLAUIBOOK_HPP

#include "Notebook.h"
#include "codelite_exports.h"

#include <wx/aui/auibook.h>

class WXDLLIMPEXP_SDK clAuiBook : public wxAuiNotebook
{
public:
    clAuiBook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = 0);
    ~clAuiBook() override;
    size_t GetAllTabs(clTabInfo::Vec_t& tabs);
    clTabHistory::Ptr_t GetHistory() const { return m_history; }
    void MoveActivePage(int newIndex);
    int SetSelection(size_t newPage) override;
    int ChangeSelection(size_t n) override;
    wxBorder GetDefaultBorder() const override;

    /// Enable events fired from the book
    void EnableEvents(bool b);
    bool AreEventsEnabled() const { return m_eventsEnabled; }
    /// Ensure that all windows exist in the history
    void UpdateHistory();
    void SetCanHaveCloseButton(bool b) { m_canHaveCloseButton = b; }
    int GetPageIndex(const wxString& name) const;
    int GetPageIndex(wxWindow* win) const;
    bool DeletePage(size_t index, bool notify);
    bool RemovePage(size_t index, bool notify);

protected:
    void OnPageClosed(wxAuiNotebookEvent& event);
    void OnPageClosing(wxAuiNotebookEvent& event);
    void OnPageChanged(wxAuiNotebookEvent& event);
    void OnPageChanging(wxAuiNotebookEvent& event);
    void OnPageRightDown(wxAuiNotebookEvent& event);
    void OnTabAreaDoubleClick(wxAuiNotebookEvent& event);
    void OnPageDoubleClick(wxAuiNotebookEvent& event);
    void OnPreferences(wxCommandEvent& event);

    void UpdatePreferences();
    void SetBookArt();

private:
    clTabHistory::Ptr_t m_history;
    bool m_eventsEnabled = true;
    bool m_canHaveCloseButton = true;
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

#endif // CLAUIBOOK_HPP
