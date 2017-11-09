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

#include "codelite_exports.h"
#include <list>
#include <wx/frame.h>
#include <wx/accel.h>
#include <vector>
#include <map>
#include <wx/menu.h>
#include <wx/event.h>
#include "macros.h"
#include "wxStringHash.h"

struct WXDLLIMPEXP_SDK MenuItemData {
    wxString resourceID;
    wxString accel;
    wxString action;
    wxString parentMenu; // For display purposes

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

struct WXDLLIMPEXP_SDK clKeyboardShortcut {
    bool m_ctrl;
    bool m_alt;
    bool m_shift;
    wxString m_keyCode;

    clKeyboardShortcut()
        : m_ctrl(false)
        , m_alt(false)
        , m_shift(false)
    {
    }

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

    typedef std::vector<clKeyboardShortcut> Vec_t;
};

class WXDLLIMPEXP_SDK clKeyboardManager : public wxEvtHandler
{
private:
    typedef std::list<wxFrame*> FrameList_t;
    MenuItemDataMap_t m_menuTable;
    MenuItemDataMap_t m_globalTable;
    wxStringSet_t m_keyCodes;
    wxStringSet_t m_allShorcuts;

protected:
    /**
     * @brief return list of frames
     */
    void DoGetFrames(wxFrame* parent, clKeyboardManager::FrameList_t& frames);
    void DoUpdateMenu(wxMenu* menu, MenuItemDataIntMap_t& accels, std::vector<wxAcceleratorEntry>& table);
    void DoUpdateFrame(wxFrame* frame, MenuItemDataIntMap_t& accels);
    void DoConvertToIntMap(const MenuItemDataMap_t& strMap, MenuItemDataIntMap_t& intMap);
    MenuItemDataMap_t DoLoadDefaultAccelerators();
    
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
    wxArrayString GetAllUnasignedKeyboardShortcuts() const;

    /**
     * @brief show a 'Add keyboard shortcut' dialog
     */
    int PopupNewKeyboardShortcutDlg(wxWindow* parent, MenuItemData& menuItemData);

    /**
     * @brief return true if the accelerator is already assigned
     */
    bool Exists(const wxString& accel) const;

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
     * For example: AddAccelerator("wxID_COPY", "Ctrl-Shift-C", "Copy the current selection");
     * @return true if the action succeeded, false otherwise
     */
    void AddGlobalAccelerator(
        const wxString& resourceID, const wxString& keyboardShortcut, const wxString& description);

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
