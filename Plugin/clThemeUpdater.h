#ifndef CLTHEMEUPDATER_H
#define CLTHEMEUPDATER_H

#include "codelite_exports.h"
#include "cl_command_event.h"
#include <vector>
#include <wx/window.h>

class WXDLLIMPEXP_SDK clThemeUpdater : public wxEvtHandler
{
    std::vector<wxWindow*> m_windows;

protected:
    void UpdateGlobalColours();

public:
    clThemeUpdater();
    ~clThemeUpdater();

    static clThemeUpdater& Get();
    void OnColoursAndFontsChanged(clCommandEvent& e);
    void RegisterWindow(wxWindow* win);
};

#endif // CLTHEMEUPDATER_H
