#ifndef CLTOOLBAR_H
#define CLTOOLBAR_H

#include "codelite_exports.h"
#include <unordered_map>
#include <vector>
#include <wx/menu.h>
#include <wx/panel.h>

class clToolBarButtonBase;
class WXDLLIMPEXP_SDK clToolBar : public wxPanel
{
    std::vector<clToolBarButtonBase*> m_buttons;
    std::vector<clToolBarButtonBase*> m_overflowButtons;
    std::vector<clToolBarButtonBase*> m_visibleButtons;
    std::unordered_map<int, wxMenu*> m_menus;
    bool m_popupShown;
    size_t m_flags;
    wxRect m_chevronRect;

public:
    enum eFlags {
        kShowLabels = (1 << 0),
    };

protected:
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnMotion(wxMouseEvent& event);
    void OnEnterWindow(wxMouseEvent& event);
    void OnLeaveWindow(wxMouseEvent& event);
    void OnOverflowItem(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
    virtual void UpdateWindowUI(long flags = wxUPDATE_UI_NONE) override;
    void DoIdleUpdate();
    wxRect CalculateRect(wxDC& dc) const;
    void EnableFlag(eFlags f, bool b)
    {
        if(b) {
            m_flags |= f;
        } else {
            m_flags &= ~f;
        }
    }
    void DoShowOverflowMenu();

public:
    clToolBar(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER,
              const wxString& name = "clToolBar");
    virtual ~clToolBar();

    /**
     * @brief set a drop down menu for a button
     */
    void SetDropdownMenu(wxWindowID buttonID, wxMenu* menu);

    /**
     * @brief find the menu for a given button
     */
    wxMenu* FindMenuById(wxWindowID buttonID) const;

    /**
     * @brief return all the buttons
     */
    std::vector<clToolBarButtonBase*>& GetButtons() { return m_buttons; };
    const std::vector<clToolBarButtonBase*>& GetButtons() const { return m_buttons; }

    /**
     * @brief display labels next to the bitmap icon
     */
    void ShowLabels(bool show) { EnableFlag(kShowLabels, show); }
    bool IsShowLabels() const { return m_flags & kShowLabels; }

    /**
     * @brief add toolbar button
     */
    clToolBarButtonBase* Add(clToolBarButtonBase* button);
    /**
     * @brief insert a button after the button identified by 'otherButton'
     * @param where insert the button after this button.
     * if where can not be found, this function returns null
     */
    clToolBarButtonBase* InsertBefore(wxWindowID where, clToolBarButtonBase* button);
    /**
     * @brief insert a button before the button identified by 'otherButton'
     * @param where insert the button after this button.
     * if where can not be found, this function returns null     */
    clToolBarButtonBase* InsertAfter(wxWindowID where, clToolBarButtonBase* button);

    clToolBarButtonBase* AddButton(wxWindowID id, const wxBitmap& bmp, const wxString& label = "");
    clToolBarButtonBase* AddMenuButton(wxWindowID id, const wxBitmap& bmp, const wxString& label = "");
    clToolBarButtonBase* AddToggleButton(wxWindowID id, const wxBitmap& bmp, const wxString& label = "");
    clToolBarButtonBase* AddSeparator();
        
    /**
     * @brief add control to the toolbar
     * @param control
     */
    clToolBarButtonBase* AddControl(wxWindow* control);
    
    // Compatibility API with wxToolBar
    clToolBarButtonBase* AddTool(wxWindowID id, const wxString& label, const wxBitmap& bmp,
                                 const wxString& helpString = "", wxItemKind kind = wxITEM_NORMAL)
    {
        wxUnusedVar(helpString);
        switch(kind) {
        case wxITEM_DROPDOWN:
            return AddMenuButton(id, bmp, label);
        case wxITEM_CHECK:
            return AddToggleButton(id, bmp, label);
        case wxITEM_NORMAL:
        default:
            return AddButton(id, bmp, label);
        }
    }

    void ToggleTool(wxWindowID buttonID, bool toggle);

    /**
     * @brief This function should be called after you have added tools.
     */
    void Realize();

    /**
     * @brief show a drop down menu for a button
     */
    void ShowMenuForButton(wxWindowID buttonID, wxMenu* menu);
    
    /**
     * @brief display a menu for a button and return the user selected menu item ID
     */
    int GetMenuSelectionFromUser(wxWindowID buttonID, wxMenu* menu);
    
    /**
     * @brief find a button by ID
     */
    clToolBarButtonBase* FindById(wxWindowID id) const;

    /**
     * @brief delete a button identified by 'id'
     * @param id the button id
     * @return true on success, false otherwise
     */
    bool DeleteById(wxWindowID id);
    // Compatibility API
    bool DeleteTool(wxWindowID id) { return DeleteById(id); }

};

#endif // CLTOOLBAR_H
