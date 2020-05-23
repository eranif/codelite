#ifndef GTKNOTEBOOK_HPP
#define GTKNOTEBOOK_HPP

#include "cl_defs.h"

#if CL_USE_NATIVEBOOK
#include "clTabHistory.h"
#include "clTabRenderer.h"
#include "cl_command_event.h"
#include <unordered_map>
#include <wx/menu.h>
#include <wx/notebook.h>
#include <gtk/gtk.h>

class Notebook : public wxNotebook
{
protected:
    size_t m_bookStyle = kNotebook_Default;

    struct UserData {
        wxString tooltip;
        wxBitmap bitmap;
    };
    std::unordered_map<wxWindow*, UserData> m_userData;
    wxMenu* m_tabContextMenu = nullptr;
    clTabHistory::Ptr_t m_history;

protected:
    void DoFinaliseAddPage(wxWindow* page, const wxString& shortlabel, const wxBitmap& bmp);
    bool GetPageDetails(wxWindow* page, int& curindex, wxString& label, int& imageId) const;
    void BindEvents();
    void Initialise(long style);
    void OnPageChanged(wxBookCtrlEvent& e);
    void OnPageChanging(wxBookCtrlEvent& e);
    wxWindow* DoUpdateHistoryPreRemove(wxWindow* page);
    void DoUpdateHistoryPostRemove(wxWindow* page, bool deletedSelection);

public:
    int FindPageByGTKHandle(WXWidget page) const;
    void TabButtonClicked(wxWindow* page);
    void TabReordered();
    void GTKLeftDClick();
    void GTKMiddleDown();
    void GTKRightDown();
    void GTKActionButtonClicked(GtkToolItem* button);

public:
    Notebook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxNotebookNameStr);
    bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxNotebookNameStr);
    // dtor
    virtual ~Notebook();

    void SetStyle(size_t bookStyle) { this->m_bookStyle = bookStyle; }
    size_t GetStyle() const { return m_bookStyle; }
    void SetTabDirection(wxDirection d);
    void EnableStyle(NotebookStyle style, bool enable);

    // API
    void AddPage(wxWindow* page, const wxString& label, bool selected = false, const wxBitmap& bmp = wxNullBitmap,
                 const wxString& shortLabel = wxEmptyString);
    bool InsertPage(size_t index, wxWindow* page, const wxString& label, bool selected = false,
                    const wxBitmap& bmp = wxNullBitmap, const wxString& shortLabel = wxEmptyString);

    bool RemovePage(size_t page, bool notify);
    bool DeletePage(size_t page, bool notify);
    bool RemovePage(size_t page) override;
    bool DeletePage(size_t page) override;

    wxWindow* GetCurrentPage() const;
    void SetPageBitmap(size_t index, const wxBitmap& bmp);
    wxBitmap GetPageBitmap(size_t index) const;
    bool DeleteAllPages() override;
    int GetPageIndex(wxWindow* window) const;
    int GetPageIndex(const wxString& label) const;
    void GetAllPages(std::vector<wxWindow*>& pages);
    clTabRenderer::Ptr_t GetArt() { return clTabRenderer::Ptr_t(nullptr); }
    void SetArt(clTabRenderer::Ptr_t art) { wxUnusedVar(art); }
    size_t GetAllTabs(clTabInfo::Vec_t& tabs);
    clTabHistory::Ptr_t GetHistory() const;
    void SetMenu(wxMenu* menu) { m_tabContextMenu = menu; }
    bool SetPageToolTip(size_t page, const wxString& tooltip);
    bool MoveActivePage(int newIndex);
    int SetSelection(size_t nPage) override;
    int ChangeSelection(size_t nPage) override;
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CHANGING, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CHANGED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CLOSING, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CLOSED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_TAB_CONTEXT_MENU, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CLOSE_BUTTON, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_TAB_DCLICKED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_TABAREA_DCLICKED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_FILELIST_BUTTON_CLICKED, clContextMenuEvent);
#endif // defined(__WXGTK__) && !defined(__WXGTK20__)
#endif // GTKNOTEBOOK_HPP
