#ifndef LLDBBREAKPOINTSPANE_H
#define LLDBBREAKPOINTSPANE_H

#include "UI.h"
#include "LLDBEvent.h"

class LLDBDebugger;
class LLDBBreakpointsPane : public LLDBBreakpointsPaneBase
{
    LLDBDebugger* m_lldb;
public:
    LLDBBreakpointsPane(wxWindow* parent, LLDBDebugger* lldb);
    virtual ~LLDBBreakpointsPane();
    
    void Initialize();
    void Clear();
    
    // Event handlers
    void OnBreakpointsUpdated(LLDBEvent &event);
    
protected:
    virtual void OnBreakpointActivated(wxDataViewEvent& event);
    virtual void OnDeleteAllBreakpoints(wxCommandEvent& event);
    virtual void OnDeleteAllBreakpointsUI(wxUpdateUIEvent& event);
    virtual void OnDeleteBreakpoint(wxCommandEvent& event);
    virtual void OnDeleteBreakpointUI(wxUpdateUIEvent& event);
    virtual void OnNewBreakpoint(wxCommandEvent& event);
    virtual void OnNewBreakpointUI(wxUpdateUIEvent& event);
};

#endif // LLDBBREAKPOINTSPANE_H
