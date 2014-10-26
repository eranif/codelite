#ifndef KEYBOARDMANAGER_H
#define KEYBOARDMANAGER_H

#include "codelite_exports.h"
#include <list>
#include <wx/frame.h>
#include <wx/accel.h>
#include <vector>

class WXDLLIMPEXP_SDK clKeyboardManager
{
    typedef std::list<wxFrame*> FrameList_t;
    std::map<int, wxString> m_accelTable;
protected:
    /**
     * @brief return list of frames
     */
    void DoGetFrames(wxFrame* parent, clKeyboardManager::FrameList_t& frames);
    void DoUpdateMenu(wxMenu* menu,
                      int actionId,
                      const wxString& keyboardShortcut,
                      std::vector<wxAcceleratorEntry>& table,
                      bool& matchFound);
    void DoUpdateFrame(wxFrame* frame, int actionId, const wxString& keyboardShortcut, bool& matchFound);
    
    clKeyboardManager();
    virtual ~clKeyboardManager();

public:
    static void Release();
    static clKeyboardManager* Get();

    /**
     * @brief add keyboard shortcut by specifying the action ID + the shortcut combination
     * For example: AddAccelerator("wxID_COPY", "Ctrl-Shift-C");
     * @return true if the action succeeded, false otherwise
     */
    bool AddAccelerator(const wxString& actionId, const wxString& keyboardShortcut);
    bool AddAccelerator(int actionId, const wxString& keyboardShortcut);
    
    /**
     * @brief append the global accelerators
     */
    void Update();
};

#endif // KEYBOARDMANAGER_H
