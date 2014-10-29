#ifndef KEYBOARDMANAGER_H
#define KEYBOARDMANAGER_H

#include "codelite_exports.h"
#include <list>
#include <wx/frame.h>
#include <wx/accel.h>
#include <vector>
#include <map>

struct WXDLLIMPEXP_SDK MenuItemData {
    int id;
    wxString accel;
    wxString action;
    wxString parentMenu; // For display purposes
    MenuItemData() : id(wxNOT_FOUND) {}
    
    struct ClearParentMenu {
        void operator()(std::pair<const int, MenuItemData> &iter) {
            iter.second.parentMenu.Clear();
        }
    };
    
    struct PrependPrefix {
        wxString m_prefix;
        PrependPrefix(const wxString& prefix) : m_prefix(prefix){}
        void operator()(std::pair<const int, MenuItemData> &iter) {
            iter.second.action.Prepend(m_prefix);
        }
    };
};

typedef std::map<int, MenuItemData> MenuItemDataMap_t;

class WXDLLIMPEXP_SDK clKeyboardManager
{
private:
    typedef std::list<wxFrame*> FrameList_t;
    MenuItemDataMap_t m_menuTable;
    MenuItemDataMap_t m_globalTable;
    
protected:
    /**
     * @brief return list of frames
     */
    void DoGetFrames(wxFrame* parent, clKeyboardManager::FrameList_t& frames);
    void
    DoUpdateMenu(wxMenu* menu, MenuItemDataMap_t& accels, std::vector<wxAcceleratorEntry>& table);
    void DoUpdateFrame(wxFrame* frame, MenuItemDataMap_t& accels);
    
    void DoGetMenuAccels(MenuItemDataMap_t& accels, wxMenu* menu, const wxString &parentMenuTitle) const;
    void DoGetFrameAccels(MenuItemDataMap_t& accels, wxFrame* frame) const;
    
    clKeyboardManager();
    virtual ~clKeyboardManager();

public:
    static void Release();
    static clKeyboardManager* Get();
    
    /**
     * @brief show a 'Add keyboard shortcut' dialog
     */
    int PopupNewKeyboardShortcutDlg(wxWindow* parent, MenuItemData& menuItemData);
    
    /**
     * @brief return true if the accelerator is already assigned
     */
    bool Exists(const wxString &accel) const;
    
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
    void AddGlobalAccelerator(int actionId, const wxString& keyboardShortcut, const wxString& description);
    
    /**
     * @brief using a menu, extract the accelerators and add them
     * as globals
     */
    void AddGlobalAccelerators(wxMenu* menu, const wxString &prefix = "");
    
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
