#ifndef CLMENUBAR_HPP
#define CLMENUBAR_HPP

#include "wxCustomControls.hpp"
#if wxUSE_NATIVE_MENUBAR
#include <wx/menu.h>
typedef wxMenuBar clMenuBar;
#else
#include <clColours.h>
#include <codelite_exports.h>
#include <unordered_map>
#include <vector>
#include <wx/control.h>
#include <wx/dc.h>
#include <wx/menu.h>
#include <wx/panel.h>

class wxMenuBar;
class WXDLLIMPEXP_SDK clMenuBar : public wxWindow
{
protected:
    struct menu_info {
        wxString full_text; // include accelerators and mnemonics
        wxString text;      // does not accelerators and mnemonics
        wxMenu* menu = nullptr;
        bool is_enabled = true;
        wxRect m_rect;
        wxRect m_text_rect;
        size_t drawing_flags = 0; // k_normal
        menu_info(const wxString& l, wxMenu* m, bool b)
            : menu(m)
            , is_enabled(b)
        {
            set_label(l);
        }

        /**
         * @brief set the menu label and update the text
         * @param label the menu label including any accelerator and mnemonics
         */
        void set_label(const wxString& label)
        {
            full_text = label;
            text = wxMenuItem::GetLabelText(label);
        }
    };

    enum e_state {
        k_normal = 0,
        k_disabled = (1 << 0),
        k_hover = (1 << 1),
        k_pressed = (1 << 2),
    };

protected:
    std::vector<menu_info> m_menus;
    clColours m_colours;
    bool m_menu_is_up = false;

protected:
    wxMenuItem* DoFindMenuItem(int id, wxMenu** parent = nullptr) const;
    menu_info* DoFindMenu(wxMenu* menu) const;
    void DoSetBestSize();

    // draw the button. flags are ORd values of e_state
    void DrawButton(wxDC& dc, size_t index, size_t flags);
    void UpdateRects(wxDC& dc);
    bool UpdateFlags(const wxPoint& pt);
    int HitTest(const wxPoint& pt) const;

protected:
    void OnPaint(wxPaintEvent& e);
    void OnEraseBg(wxEraseEvent& e);
    void OnLeftDown(wxMouseEvent& e);
    void OnLeftUp(wxMouseEvent& e);
    void OnMotion(wxMouseEvent& e);
    void OnEnterWindow(wxMouseEvent& e);
    void OnLeaveWindow(wxMouseEvent& e);
    bool DoAppend(wxMenu* menu, const wxString& title);

public:
    clMenuBar();
    clMenuBar(wxWindow* parent, size_t n, wxMenu* menus[], const wxString titles[], long style = 0);
    virtual ~clMenuBar();
    /**
     * @brief Adds the item to the end of the menu bar.
     */
    virtual bool Append(wxMenu* menu, const wxString& title);

    /**
     * @brief Checks or unchecks a menu item
     */
    void Check(int id, bool check);

    /**
     * @brief Enables or disables (greys out) a menu item
     */
    void Enable(int id, bool enable);

    /**
     * @brief Returns true if the menu with the given index is enabled
     */
    bool IsEnabledTop(size_t pos) const;

    /**
     * @brief Enables or disables a whole menu
     */
    virtual void EnableTop(size_t pos, bool enable);
    /**
     * @brief Finds the menu item object associated with the given menu item identifier
     */
    virtual wxMenuItem* FindItem(int id, wxMenu** menu = NULL) const;
    /**
     * @brief Returns the index of the menu with the given title or wxNOT_FOUND if no such menu exists in this menubar
     */
    int FindMenu(const wxString& title) const;
    /**
     * @brief Gets the help string associated with the menu item identifier
     */
    wxString GetHelpString(int id) const;
    /**
     * @brief Gets the label associated with a menu item
     */
    wxString GetLabel(int id) const;
    /**
     * @brief Returns the label of a top-level menu
     */
    wxString GetLabelTop(size_t pos) const;
    /**
     * @brief Returns the menu at menuIndex (zero-based)
     */
    wxMenu* GetMenu(size_t menuIndex) const;
    /**
     * @brief Returns the number of menus in this menubar
     */
    size_t GetMenuCount() const;
    /**
     * @brief Returns the label of a top-level menu
     */
    virtual wxString GetMenuLabel(size_t pos) const;
    /**
     * @brief Returns the label of a top-level menu
     */
    virtual wxString GetMenuLabelText(size_t pos) const;
    /**
     * @brief Inserts the menu at the given position into the menu bar
     */
    virtual bool Insert(size_t pos, wxMenu* menu, const wxString& title);
    /**
     * @brief Determines whether an item is checked.
     */
    bool IsChecked(int id) const;
    /**
     * @brief Determines whether an item is enabled.
     */
    bool IsEnabled(int id) const;
    /**
     * @brief Removes the menu from the menu bar and returns the menu object - the caller is responsible for deleting it
     */
    virtual wxMenu* Remove(size_t pos);
    /**
     * @brief Replaces the menu at the given position with another one
     */
    virtual wxMenu* Replace(size_t pos, wxMenu* menu, const wxString& title);
    /**
     * @brief Sets the help string associated with a menu item.
     */
    void SetHelpString(int id, const wxString& helpString);
    /**
     * @brief Sets the label of a menu item
     */
    void SetLabel(int id, const wxString& label);
    /**
     * @brief Sets the label of a top-level menu
     */
    void SetLabelTop(size_t pos, const wxString& label);
    /**
     * @brief Sets the label of a top-level menu
     */
    virtual void SetMenuLabel(size_t pos, const wxString& label);

    /**
     * @brief update the accelerator table for the parent based on the installed menus
     */
    void UpdateAccelerators();

    // Customisation point: allow user to set colours
    void SetColours(const clColours& colours);
    const clColours& GetColours() const { return m_colours; }

    /**
     * @brief built menu bar from wxMenuBar. Note that this class takes ownership over the menus
     * and "strip" the wxMenuBar from all the menus attached to it
     */
    void FromMenuBar(wxMenuBar* mb);
};
#endif
#endif // CLMENUBAR_HPP
