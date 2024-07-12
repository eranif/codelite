//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clKeyboardManager.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef KEYBOARDMANAGER_H
#define KEYBOARDMANAGER_H

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "macros.h"
#include "wxStringHash.h"

#include <list>
#include <map>
#include <set>
#include <vector>
#include <wx/accel.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/menu.h>

class WXDLLIMPEXP_SDK clKeyboardShortcut
{
    wxKeyCode m_control_type = WXK_NONE;
    bool m_alt = false;
    bool m_shift = false;
    wxString m_keyCode;

    /**
     * @brief tokenize the accelerator string
     */
    wxArrayString Tokenize(const wxString& accelString) const;

protected:
    wxString to_string(bool for_ui) const;

public:
    clKeyboardShortcut() {}
    explicit clKeyboardShortcut(wxKeyCode ctrl, bool alt, bool shift, const wxString& keyCode)
        : m_control_type(ctrl)
        , m_alt(alt)
        , m_shift(shift)
        , m_keyCode(keyCode)
    {
    }
    clKeyboardShortcut(const char* accelString) { FromString(wxString(accelString)); }
    clKeyboardShortcut(const wxString& accelString) { FromString(accelString); }

    /**
     * @brief please use the FromString() instead
     */
    clKeyboardShortcut& operator=(const char* accelString) = delete;
    clKeyboardShortcut& operator=(const wxString& accelString) = delete;

    /// The control key code (can be WXK_CONTROL, WXK_RAW_CONTROL or WXK_NONE)
    wxKeyCode GetControl() const { return m_control_type; }

    /// Is the Alt key part of the accelerator?
    bool GetAlt() const { return IsOk() && m_alt; }

    /// Is the Shift key part of the accelerator?
    bool GetShift() const { return IsOk() && m_shift; }

    /// The accelerator key code (e.g. 'C')
    const wxString& GetKeyCode() const { return m_keyCode; }

    /**
     * @brief equality operators
     */
    bool operator==(const clKeyboardShortcut& rhs) const;
    bool operator!=(const clKeyboardShortcut& rhs) const { return !(*this == rhs); }

    /**
     * @brief 'less than' operator (may be used in STL containers)
     */
    bool operator<(const clKeyboardShortcut& rhs) const;

    /**
     * @brief returns true if this shortcut is valid, false otherwise
     */
    bool IsOk() const;

    /**
     * @brief clear this accelerator
     */
    void Clear();

    /**
     * @brief construct this object from string representation
     * e.g.: Ctrl-Alt-1
     */
    void FromString(const wxString& accelString);
    /**
     * @brief return a string representation of this accelerator
     */
    wxString ToString() const;

    /// Similar to `ToString` but convert `rawctrl` & `ctrl` into `Ctrl` & `Cmd` accordingly
    wxString DisplayString() const;

    using Vec_t = std::vector<clKeyboardShortcut>;
    using Set_t = std::set<clKeyboardShortcut>;
};

struct WXDLLIMPEXP_SDK MenuItemData {
    wxString resourceID;
    wxString parentMenu; // For display purposes
    wxString action;
    clKeyboardShortcut accel;

    struct ClearParentMenu {
        void operator()(std::pair<const int, MenuItemData>& iter) { iter.second.parentMenu.Clear(); }
    };

    struct PrependPrefix {
        wxString m_prefix;
        PrependPrefix(const wxString& prefix)
            : m_prefix(prefix)
        {
        }
        void operator()(std::pair<const int, MenuItemData>& iter) { iter.second.action.Prepend(m_prefix); }
    };
};

typedef std::unordered_map<wxString, MenuItemData> MenuItemDataMap_t;
typedef std::unordered_map<int, MenuItemData> MenuItemDataIntMap_t;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_KEYBOARD_ACCEL_INIT_DONE, clCommandEvent);
class WXDLLIMPEXP_SDK clKeyboardManager : public wxEvtHandler
{
private:
    typedef std::list<wxFrame*> FrameList_t;
    bool m_initialized = false;
    MenuItemDataMap_t m_accelTable;        // a set of accelerators configured by user
    MenuItemDataMap_t m_defaultAccelTable; // a set of default accelerators
    wxStringSet_t m_keyCodes;
    clKeyboardShortcut::Set_t m_allShortcuts;

    /**
     * @brief an internal struct used by the 2nd AddAccelerator() overload
     */
    struct AddAccelData {
        AddAccelData(const wxString& resourceID, const wxString& action, const clKeyboardShortcut& accel = {})
            : m_resourceID(resourceID)
            , m_action(action)
            , m_accel(accel)
        {
        }
        wxString m_resourceID;
        wxString m_action;
        clKeyboardShortcut m_accel;
    };

protected:
    /**
     * @brief return list of frames
     */
    void DoGetFrames(wxFrame* parent, clKeyboardManager::FrameList_t& frames);
    void DoUpdateMenu(wxMenu* menu, MenuItemDataIntMap_t& accels, std::vector<wxAcceleratorEntry>& table);
    void DoUpdateFrame(wxFrame* frame, MenuItemDataIntMap_t& accels);
    void DoConvertToIntMap(const MenuItemDataMap_t& strMap, MenuItemDataIntMap_t& intMap);
    MenuItemDataMap_t DoLoadAccelerators(const wxFileName& filename) const;

    clKeyboardManager();
    virtual ~clKeyboardManager();

protected:
    void OnStartupCompleted(wxCommandEvent& event);

public:
    static void Release();
    static clKeyboardManager* Get();

    /**
     * @brief return an array of all unassigned keyboard shortcuts
     */
    clKeyboardShortcut::Vec_t GetAllUnassignedKeyboardShortcuts() const;

    /**
     * @brief show a 'Add keyboard shortcut' dialog
     */
    int PopupNewKeyboardShortcutDlg(wxWindow* parent, MenuItemData& menuItemData);

    /**
     * @brief return true if the accelerator is already assigned
     */
    bool Exists(const clKeyboardShortcut& accel) const;

    /**
     * @brief save the bindings to disk
     */
    void Save();

    /**
     * @brief load bindings from the file system
     */
    void Initialize();

    /**
     * @brief add keyboard shortcut by specifying the action ID + the shortcut combination
     * For example: AddAccelerator("wxID_COPY", _("Edit"), _("Copy the current selection"), "Ctrl-Shift-C");
     */
    void AddAccelerator(const wxString& resourceID, const wxString& parentMenu, const wxString& action,
                        const clKeyboardShortcut& accel = {});

    /**
     * @brief a convenience overload for AddAccelerator() method
     * For example:
     *    AddAccelerator(_("File"), { { "wxID_OPEN", _("Open"), "Ctrl-O" }, { "wxID_CLOSE", _("Close"), "Ctrl-W" } });
     * is equivalent to:
     *    AddAccelerator("wxID_OPEN", _("File"), _("Open"), "Ctrl-O");
     *    AddAccelerator("wxID_CLOSE", _("File"), _("Close"), "Ctrl-W");
     */
    void AddAccelerator(const wxString& parentMenu, const std::vector<AddAccelData>& table);

    /**
     * @brief replace all acceleratos with 'accels'
     */
    void SetAccelerators(const MenuItemDataMap_t& accels);

    /**
     * @brief return all accelerators known to CodeLite
     */
    void GetAllAccelerators(MenuItemDataMap_t& accels) const;

    /**
     * @brief update accelerators
     */
    void Update(wxFrame* frame = NULL);

    /**
     * @brief restore keyboard shortcuts to defaults
     */
    void RestoreDefaults();
};

#endif // KEYBOARDMANAGER_H
