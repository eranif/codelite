#ifndef CLAUINOTEBOOK_H
#define CLAUINOTEBOOK_H

#include "cl_defs.h"
#if USE_AUI_NOTEBOOK

#include "clTab.h"
#include "clTabHistory.h"
#include "clTabRenderer.h"
#include "codelite_exports.h"
#include <vector>
#include <wx/aui/auibook.h>

class WXDLLIMPEXP_SDK clAuiNotebook : public wxAuiNotebook
{
private:
    clTabHistory::Ptr_t m_history;

protected:
    void OnAuiPageChanging(wxAuiNotebookEvent& evt);
    void OnAuiPageChanged(wxAuiNotebookEvent& evt);
    void OnAuiPageClosing(wxAuiNotebookEvent& evt);
    void OnAuiPageClosed(wxAuiNotebookEvent& evt);
    void OnAuiTabContextMenu(wxAuiNotebookEvent& evt);
    void OnTabCloseButton(wxAuiNotebookEvent& evt);
    void OnTabMiddleClicked(wxAuiNotebookEvent& evt);

public:
    clAuiNotebook(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxEmptyString);
    ~clAuiNotebook();

    // Backward compatiblity API
    long GetStyle() const { return GetWindowStyle(); }
    void SetStyle(size_t style) { SetWindowStyle(style); }
    void EnableStyle(NotebookStyle style, bool b)
    {
        if(b) {
            SetWindowStyle(GetWindowStyle() | style);
        } else {
            SetWindowStyle(GetWindowStyle() & ~style);
        }
    }
    int GetPageIndex(const wxString& label) const;
    int GetPageIndex(wxWindow* page) const;
    clTabHistory::Ptr_t GetHistory() { return m_history; }
    wxArrayString GetAllTabsLabels();
    void SetTabDirection(wxDirection d);
    void GetAllTabs(clTab::Vec_t& tabs);
    
    bool DeletePage(size_t page);
    bool RemovePage(size_t page);
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CHANGING, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CHANGED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CLOSING, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CLOSED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CLOSE_BUTTON, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_TAB_DCLICKED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_NAVIGATING, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_TABAREA_DCLICKED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_TAB_CONTEXT_MENU, wxBookCtrlEvent);

typedef clAuiNotebook Notebook;

#endif // #if USE_AUI_NOTEBOOK
#endif // CLAUINOTEBOOK_H
