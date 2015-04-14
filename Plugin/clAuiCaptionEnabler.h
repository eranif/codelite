#ifndef CLAUICAPTIONENABLER_H
#define CLAUICAPTIONENABLER_H

#include "codelite_exports.h"
#include <wx/aui/framemanager.h>
#include <wx/event.h>
#include <wx/timer.h>

class WXDLLIMPEXP_SDK clAuiCaptionEnabler : public wxEvtHandler
{
    wxAuiManager* m_aui;
    wxWindow* m_window;
    wxTimer* m_timer;
    bool m_captionAdded;
    wxString m_caption;
    bool m_ensureCaptionsVisible;
    
protected:
    void OnTimer(wxTimerEvent& event);
    void OnEditorSettingsChanged(wxCommandEvent& event);

public:
    void Initialize(wxWindow* window, const wxString& windowCaption, wxAuiManager* aui);
    clAuiCaptionEnabler();
    virtual ~clAuiCaptionEnabler();
};

#endif // CLAUICAPTIONENABLER_H
