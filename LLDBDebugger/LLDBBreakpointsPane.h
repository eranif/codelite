#ifndef LLDBBREAKPOINTSPANE_H
#define LLDBBREAKPOINTSPANE_H

#include "UI.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBProtocol/LLDBBreakpoint.h"
#include "LLDBProtocol/LLDBConnector.h"

class LLDBDebugger;
class LLDBPlugin;
class LLDBBreakpointsPane : public LLDBBreakpointsPaneBase
{
    LLDBPlugin *m_plugin;
    LLDBConnector* m_connector;
    
public:
    LLDBBreakpointsPane(wxWindow* parent, LLDBPlugin *plugin);
    virtual ~LLDBBreakpointsPane();
    
    void Initialize();
    void Clear();
    
    // Event handlers
    void OnBreakpointsUpdated(LLDBEvent &event);
    LLDBBreakpoint::Ptr_t GetBreakpoint(const wxDataViewItem& item);
protected:
    void GotoBreakpoint(LLDBBreakpoint::Ptr_t bp);
    
protected:
    virtual void OnDeleteAll(wxCommandEvent& event);
    virtual void OnDeleteAllUI(wxUpdateUIEvent& event);
    virtual void OnDeleteBreakpoint(wxCommandEvent& event);
    virtual void OnDeleteBreakpointUI(wxUpdateUIEvent& event);
    virtual void OnNewBreakpoint(wxCommandEvent& event);
    virtual void OnNewBreakpointUI(wxUpdateUIEvent& event);
    virtual void OnBreakpointActivated(wxDataViewEvent& event);
};

#endif // LLDBBREAKPOINTSPANE_H
