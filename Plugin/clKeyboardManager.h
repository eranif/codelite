#ifndef KEYBOARDMANAGER_H
#define KEYBOARDMANAGER_H

#include "codelite_exports.h"
#include <list>
#include <wx/frame.h>
#include <wx/accel.h>
#include <vector>
#include <map>

class WXDLLIMPEXP_SDK clKeyboardManager
{
public:
    struct KeyBinding {
        typedef std::map<int, clKeyboardManager::KeyBinding> Map_t;

        int actionId;
        wxString accelerator;
        wxString description;
        KeyBinding()
            : actionId(wxNOT_FOUND)
        {
        }
    };

private:
    typedef std::list<wxFrame*> FrameList_t;
    clKeyboardManager::KeyBinding::Map_t m_accelTable;

protected:
    /**
     * @brief return list of frames
     */
    void DoGetFrames(wxFrame* parent, clKeyboardManager::FrameList_t& frames);
    void
    DoUpdateMenu(wxMenu* menu, clKeyboardManager::KeyBinding::Map_t& accels, std::vector<wxAcceleratorEntry>& table);
    void DoUpdateFrame(wxFrame* frame, clKeyboardManager::KeyBinding::Map_t& accels);

    clKeyboardManager();
    virtual ~clKeyboardManager();

public:
    static void Release();
    static clKeyboardManager* Get();
    
    /**
     * @brief save the bindings to disk
     */
    void Save();
    
    /**
     * @brief load bindings from the file system
     */
    void Load();
    
    /**
     * @brief add keyboard shortcut by specifying the action ID + the shortcut combination
     * For example: AddAccelerator("wxID_COPY", "Ctrl-Shift-C", "Copy the current selection");
     * @return true if the action succeeded, false otherwise
     */
    bool AddAccelerator(const wxString& actionId, const wxString& keyboardShortcut, const wxString& description);
    bool AddAccelerator(int actionId, const wxString& keyboardShortcut, const wxString& description);
    /**
     * @brief add list of accelerators
     */
    bool AddAccelerators(clKeyboardManager::KeyBinding::Map_t& accels);
};

#endif // KEYBOARDMANAGER_H
