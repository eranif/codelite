#ifndef CLSYSTEMSETTINGS_H
#define CLSYSTEMSETTINGS_H

#include "clColours.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <wx/event.h>
#include <wx/settings.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SYS_COLOURS_CHANGED, clCommandEvent);

enum wxSysColourEx {
    wxSYS_COLOUR_TOOLBAR = wxSYS_COLOUR_MAX + 1,
    wxSYS_COLOUR_TOOLBARTEXT,
};

class WXDLLIMPEXP_SDK clSystemSettings : public wxEvtHandler, public wxSystemSettings
{
public:
    static clSystemSettings& Get();

    virtual ~clSystemSettings();
    static wxColour GetColour(int index);
    /**
     * @brief return the default panel colour. On Windows, this returns
     * wxSYS_COLOUR_3DFACE while on other platforms it returns wxSYS_COLOUR_WINDOW
     */
    static wxColour GetDefaultPanelColour();

    static bool IsDark();

protected:
    clSystemSettings();
    void OnAppActivated(wxActivateEvent& event);
    void OnColoursChanged(clCommandEvent& event);
    void OnSystemColourChanged(wxSysColourChangedEvent& event);
    void DoColourChangedEvent();

private:
    static wxColour btn_face;
    static wxColour panel_face;
};

#endif // CLSYSTEMSETTINGS_H
