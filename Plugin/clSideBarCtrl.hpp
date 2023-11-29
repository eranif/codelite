#ifndef CLSIDEBARCTRL_HPP
#define CLSIDEBARCTRL_HPP

#include "codelite_exports.h"

#include <wx/bitmap.h>
#include <wx/control.h>
#include <wx/simplebook.h>

class SideBarButton;
class WXDLLIMPEXP_SDK clSideBarButtonCtrl : public wxControl
{
    friend class SideBarButton;
    friend class clSideBarCtrl;

protected:
    wxBoxSizer* m_mainSizer = nullptr;
    wxDirection m_buttonsPosition = wxLEFT;

protected:
    void MoveAfter(SideBarButton* src, SideBarButton* target);
    void MoveBefore(SideBarButton* src, SideBarButton* target);
    int GetButtonIndex(SideBarButton* btn) const;
    std::vector<wxWindow*> GetAllButtons();
    wxWindow* DoChangeSelection(int pos, bool notify);
    void OnPaint(wxPaintEvent& event);
    void Initialise();

public:
    clSideBarButtonCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clSideBarButtonCtrl();

    /// Set the buttons packing (horizontal or vertical)
    bool IsHorizontalLayout() const;
    void SetButtonsPosition(wxDirection direction);

    /// Add new button at the end, returns its index
    /// Note that the `label` property is used as the tooltip
    /// It is here for convenience: it can be used to fetch
    int AddButton(const wxBitmap bmp, const wxString& label, wxWindow* linked_page, bool select = false);

    /// Remove a button by index
    /// return the removed button linked page
    wxWindow* RemoveButton(int pos);

    /// Return the current selection
    int GetSelection() const;

    /// Change selection -> this call fires an event
    /// return the new selection linked page
    wxWindow* SetSelection(int pos);

    /// Change selection -> no event is fired
    /// return the new selection linked page
    wxWindow* ChangeSelection(int pos);

    /// How many buttons do we have in this control?
    size_t GetButtonCount() const;

    /// Return the page position
    int GetPageIndex(wxWindow* page) const;

    /// Return the page position
    int GetPageIndex(const wxString& label) const;

    /// return the linked page for the current selection
    wxWindow* GetSelectionLinkedPage() const;

    /// Return the button at position `pos`
    SideBarButton* GetButton(size_t pos) const;

    /// Return the button identified by `label`
    SideBarButton* GetButton(const wxString& label) const;

    /// Remove all buttons
    void Clear();
};

class WXDLLIMPEXP_SDK clSideBarCtrl : public wxPanel
{
    clSideBarButtonCtrl* m_buttons = nullptr;
    wxSimplebook* m_book = nullptr;
    wxDirection m_buttonsPosition = wxLEFT;
    wxBoxSizer* m_mainSizer = nullptr;

protected:
    /// Return the page position
    int SimpleBookGetPageIndex(wxWindow* page) const;
    void DoRemovePage(size_t pos, bool delete_it);
    void PlaceButtons();
    void OnSize(wxSizeEvent& event);

public:
    clSideBarCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clSideBarCtrl();

    /// Book API
    void AddPage(wxWindow* page, const wxString& label, wxBitmap bmp, bool selected = false);

    /// Move page identified by `label` to a new position
    void MovePageToIndex(const wxString& label, int new_pos);

    /// return the number of pages in the control
    size_t GetPageCount() const;

    /// return page by index
    wxWindow* GetPage(size_t pos) const;

    /// return page by its label
    wxWindow* GetPage(const wxString& label) const;

    /// return page label
    wxString GetPageText(size_t pos) const;

    /// return the page bitmap
    wxBitmap GetPageBitmap(size_t pos) const;

    /// Remove page (this does not delete it)
    void RemovePage(size_t pos);

    /// Remove all pages, do not destroy them
    void RemoveAll();

    /// Delete page
    void DeletePage(size_t pos);

    /// Change selection and fire an event
    void SetSelection(size_t pos);

    /// Change selection - no event is fired
    void ChangeSelection(size_t pos);

    /// return the current selection
    int GetSelection() const;

    /// Return the page position
    int GetPageIndex(wxWindow* page) const;

    /// Return the page position
    int GetPageIndex(const wxString& label) const;

    /// place the buttons position (top, left, right or bottom)
    void SetButtonPosition(wxDirection direction);
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SIDEBAR_SELECTION_CHANGED, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SIDEBAR_CONTEXT_MENU, wxContextMenuEvent);
#endif // CLSIDEBARCTRL_HPP
