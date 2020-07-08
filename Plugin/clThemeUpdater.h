#ifndef CLTHEMEUPDATER_H
#define CLTHEMEUPDATER_H

#include "cl_command_event.h"
#include "codelite_exports.h"
#include <unordered_set>
#include <vector>
#include <wx/window.h>

class WXDLLIMPEXP_SDK clThemeUpdater : public wxEvtHandler
{
    std::unordered_set<wxWindow*> m_windows;

protected:
    void UpdateGlobalColours();
    void OnWindowDestroyed(wxWindowDestroyEvent& event);

public:
    clThemeUpdater();
    ~clThemeUpdater();

    static clThemeUpdater& Get();
    void OnColoursAndFontsChanged(clCommandEvent& e);
    void RegisterWindow(wxWindow* win);
    void UnRegisterWindow(wxWindow* win);
};

#endif // CLTHEMEUPDATER_H
