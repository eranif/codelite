#ifndef PHPDEBUGPANE_H
#define PHPDEBUGPANE_H

#include "php_ui.h"
#include <cl_command_event.h>
#include "XDebugManager.h"
#include "xdebugevent.h"
#include <macros.h>

class PHPDebugPane : public PHPDebugPaneBase
{
    PHPDebuggerImages m_images;

    
public:
    PHPDebugPane(wxWindow* parent);
    virtual ~PHPDebugPane();
    void OnUpdateStackTrace(XDebugEvent &e);
    void OnRefreshBreakpointsView(XDebugEvent &e);
    void OnXDebugSessionEnded(XDebugEvent &e);
    void SelectTab(const wxString &title);

protected:
    virtual void OnBreakpointItemActivated(wxDataViewEvent& event);
    XDebugBreakpoint GetBreakpoint(const wxDataViewItem& item) const;

protected:
    virtual void OnClearAll(wxCommandEvent& event);
    virtual void OnClearAllUI(wxUpdateUIEvent& event);
    virtual void OnDeleteBreakpoint(wxCommandEvent& event);
    virtual void OnDeleteBreakpointUI(wxUpdateUIEvent& event);
    virtual void OnCallStackItemActivated(wxDataViewEvent& event);
};
#endif // PHPDEBUGPANE_H
