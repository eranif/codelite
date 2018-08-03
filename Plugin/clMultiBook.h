#ifndef CLMULTIBOOK_H
#define CLMULTIBOOK_H

#include "clTabHistory.h"
#include <Notebook.h>
#include <codelite_exports.h>
#include <vector>
#include <wx/bitmap.h>
#include <wx/splitter.h>

class WXDLLIMPEXP_SDK clMultiBook : public wxPanel
{
    Notebook* m_leftBook;
    Notebook* m_rightBook;

    wxSplitterWindow* m_splitter;
    size_t m_style;
    int m_selection;
    clTabHistory::Ptr_t m_history;
    wxWindow* m_defaultPage;

protected:
    bool GetActiveBook(Notebook** book, size_t& bookIndex) const;
    bool GetBookByPageIndex(size_t pageIndex, Notebook** book, size_t& bookIndex, size_t& modPageIndex) const;
    bool GetActivePageBook(Notebook** book, size_t& bookIndex, size_t& modPageIndex) const;
    void MovePageToNotebook(Notebook* srcbook, size_t index, Notebook* destbook);
    void UpdateView();
    int BookIndexToGlobalIndex(size_t bookIndex, size_t pageIndex) const;
    int BookIndexToGlobalIndex(Notebook* book, size_t pageIndex) const;

    // Notebook* AddNotebook();
    Notebook* CreateNotebook(wxWindow* parent);

    bool IsOurNotebook(Notebook* book) const;
    void DoShowWindow(wxWindow* win, bool show);
    void ShowNotebook();

protected:
    void OnEventProxy(wxBookCtrlEvent& event);
    void OnFocus(wxFocusEvent& e);

public:
    clMultiBook(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxEmptyString);
    virtual ~clMultiBook();
    
    /**
     * @brief display the welcome pahe
     */
    void ShowDefaultPage(bool show = true);
    
    /**
     * @brief register a default page to be displayed when there are no
     * more open tabs
     */
    void SetDefaultPage(wxWindow* page);
    clTabHistory::Ptr_t GetHistory() { return m_history; }

    /**
     * @brief move page one notebook right
     */
    void MoveToRightTabGroup();

    /**
     * @brief move page one notebook left
     */
    void MoveToLeftTabGroup();

    /**
     * @brief can we move the active tab to left tab group?
     */
    bool CanMoveToTabGroupLeft() const;

    /**
     * @brief can we move the active tab to the right tab group?
     */
    bool CanMoveToTabGroupRight() const;

    //----------------------------
    // Notebook compatiblity API
    //----------------------------
    /**
     * @brief append page to the notebook
     */
    void AddPage(wxWindow* page, const wxString& label, bool selected = false, const wxBitmap& bmp = wxNullBitmap);

    /**
     * @brief insert page at a specified position
     */
    bool InsertPage(size_t index, wxWindow* page, const wxString& label, bool selected = false,
                    const wxBitmap& bmp = wxNullBitmap);
    /**
     * @brief Returns the string for the given page
     */
    wxWindow* GetPage(size_t index) const;

    /**
     * @brief Deletes the specified page and the associated window
     */
    bool DeletePage(size_t page, bool notify = true);

    /**
     * @brief return the currently selected page or null
     */
    wxWindow* GetCurrentPage() const;

    /**
     * @brief Returns the number of pages in the control
     */
    size_t GetPageCount() const;

    /**
     * @brief return the current selection. return wxNOT_FOUND if non is selected
     */
    int GetSelection() const;

    /**
     * @brief return all tabs info
     * @param tabs [output]
     */
    size_t GetAllTabs(clTabInfo::Vec_t& tabs);

    /**
     * @brief Sets the tool tip displayed when hovering over the tab label of the page
     * @return true if tool tip was updated, false if it failed, e.g. because the page index is invalid.
     */
    bool SetPageToolTip(size_t page, const wxString& tooltip);

    /**
     * @brief update the selected tab. This function also fires an event
     */
    int SetSelection(size_t tabIdx);

    /**
     * @brief Sets the text for the given page.
     */
    bool SetPageText(size_t page, const wxString& text);

    /**
     * @brief Returns the string for the given page
     */
    wxString GetPageText(size_t page) const;
    /**
     * @brief return the page bitmap
     * @param page
     * @return
     */
    wxBitmap GetPageBitmap(size_t page) const;

    /**
     * @brief Deletes all pages
     */
    bool DeleteAllPages();

    /**
     * @brief set the notebook style. The style bits are kNotebook_* (you can set several
     * styles OR-ed)
     */
    void SetStyle(size_t style);

    /**
     * @brief return the book style
     */
    size_t GetStyle() const;

    /**
     * @brief return the index of a given window in the tab control
     * @param window
     * @return return window index, or wxNOT_FOUND
     */
    int GetPageIndex(wxWindow* window) const;

    /**
     * @brief return the index of a given window by its title
     */
    int GetPageIndex(const wxString& label) const;

    /**
     * @brief set the image for the given page
     */
    void SetPageBitmap(size_t index, const wxBitmap& bmp);

    /**
     * @brief Deletes the specified page, without deleting the associated window
     */
    bool RemovePage(size_t page, bool notify = false);

    /**
     * @brief move the active page and place it in the new nexIndex
     */
    bool MoveActivePage(int newIndex);
};

#endif // CLMULTIBOOK_H
