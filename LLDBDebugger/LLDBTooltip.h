#ifndef LLDBTOOLTIP_H
#define LLDBTOOLTIP_H

#include "UI.h"
#include "LLDBProtocol/LLDBVariable.h"
#include "LLDBProtocol/LLDBEvent.h"

class LLDBPlugin;

// ----------------------------------------------------------
// ----------------------------------------------------------

class LLDBTooltip : public LLDBTooltipBase
{
    LLDBPlugin *m_plugin;
    std::map<int, wxTreeItemId> m_itemsPendingExpansion;
    bool m_dragging;
    
private:
    void DoCleanup();
    LLDBVariableClientData* ItemData( const wxTreeItemId & item ) const;
    void DoAddVariable(const wxTreeItemId& parent, LLDBVariable::Ptr_t variable);
    void DoUpdateSize(bool performClean);
    
public:
    LLDBTooltip(wxWindow* parent, LLDBPlugin* plugin);
    virtual ~LLDBTooltip();

    void Show(const wxString &displayName, LLDBVariable::Ptr_t variable);

protected:
    virtual void OnStatusEnterWindow(wxMouseEvent& event);
    virtual void OnStatusLeaveWindow(wxMouseEvent& event);
    virtual void OnCheckMousePosition(wxTimerEvent& event);
    virtual void OnItemExpanding(wxTreeEvent& event);
    virtual void OnStatusBarLeftDown(wxMouseEvent& event);
    virtual void OnStatusBarLeftUp(wxMouseEvent& event);
    virtual void OnCaptureLost(wxMouseCaptureLostEvent& e);
    virtual void OnStatusBarMotion(wxMouseEvent& event);

    // LLDB events
    void OnLLDBVariableExpanded(LLDBEvent &event);
};
#endif // LLDBTOOLTIP_H
