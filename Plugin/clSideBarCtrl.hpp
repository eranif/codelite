#ifndef CLSIDEBARCTRL_HPP
#define CLSIDEBARCTRL_HPP

#include "codelite_exports.h"

#include <wx/aui/auibar.h>
#include <wx/bitmap.h>
#include <wx/control.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/simplebook.h>
#include <wx/sizer.h>
#include <wx/toolbar.h>

#if defined(__WXMSW__) || defined(__WXGTK__)
#define USE_NATIVETOOLBAR 1
#else
#define USE_NATIVETOOLBAR 0
#endif

#if USE_NATIVETOOLBAR
struct LongClientData : wxObject {
    long m_data = wxNOT_FOUND;
    LongClientData(long l)
        : m_data(l)
    {
    }
    ~LongClientData() override {}
};

class SideBarToolBar : public wxToolBar
{
public:
    SideBarToolBar(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
    size_t GetToolCount() const { return this->GetToolsCount(); }
    wxToolBarToolBase* FindToolByIndex(size_t pos) { return GetToolByPos(pos); }
    void SetUserData(wxToolBarToolBase* tool, long data)
    {
        LongClientData* cd = new LongClientData(data);
        tool->SetClientData(cd);
    }
    long GetUserData(wxToolBarToolBase* tool)
    {
        LongClientData* cd = reinterpret_cast<LongClientData*>(tool->GetClientData());
        return cd->m_data;
    }
    wxToolBarToolBase* FindTool(int tool_id)
    {
        for (size_t i = 0; i < GetToolsCount(); ++i) {
            auto tool = GetToolByPos(i);
            if (tool->GetId() == tool_id) {
                return tool;
            }
        }
        return nullptr;
    }
};
#else
typedef wxAuiToolBar SideBarToolBar;
#endif

struct WXDLLIMPEXP_SDK clSideBarToolData : public wxClientData {
    clSideBarToolData(const wxString& d)
        : data(d)
    {
    }
    wxString data;
};

class WXDLLIMPEXP_SDK clSideBarCtrl : public wxControl
{
    SideBarToolBar* m_toolbar = nullptr;
    wxSimplebook* m_book = nullptr;
    wxDirection m_buttonsPosition = wxLEFT;
    wxBoxSizer* m_mainSizer = nullptr;
    std::unordered_map<long, clSideBarToolData> m_toolDataMap;
    int m_selectedToolId = wxNOT_FOUND;

protected:
    /// Return the page position
    int SimpleBookGetPageIndex(wxWindow* page) const;
    void DoRemovePage(size_t pos, bool delete_it);
    void PlaceButtons();
    void OnSize(wxSizeEvent& event);
    void AddTool(const wxString& label, const wxString& bmpname, size_t book_index);
    void OnDPIChangedEvent(wxDPIChangedEvent& event);
    void OnContextMenu(
#if USE_NATIVETOOLBAR
        wxCommandEvent& event
#else
        wxAuiToolBarEvent& event
#endif
    );

    const clSideBarToolData* GetToolData(long id) const;
    /// add tool data, return its unique ID
    long AddToolData(clSideBarToolData data);
    void DeleteToolData(long id);
    void ClearAllToolData();
    void MSWUpdateToolbarBitmaps(int new_selection, int old_selection);
    int GetToolIdForBookPos(int book_index) const;

public:
    clSideBarCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clSideBarCtrl();

    /// Book API
    void AddPage(wxWindow* page, const wxString& label, const wxString& bmpname, bool selected = false);

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
    wxString GetPageBitmap(size_t pos) const;

    /// Set the page bitmap
    void SetPageBitmap(size_t pos, const wxString& bmp);

    /// Remove page (this does not delete it)
    void RemovePage(size_t pos);

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

    void Realize();
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SIDEBAR_SELECTION_CHANGED, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SIDEBAR_CONTEXT_MENU, wxContextMenuEvent);
#endif // CLSIDEBARCTRL_HPP
