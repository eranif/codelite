#ifndef LLDBLOCALSVIEW_H
#define LLDBLOCALSVIEW_H

#include "UI.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBProtocol/LLDBLocalVariable.h"
#include "cl_treelistctrl.h"
#include <map>

class LLDBDebuggerPlugin;
class LLDBLocalsView : public LLDBLocalsViewBase
{
    typedef std::map<int, wxTreeItemId> IntItemMap_t;
    
    LLDBDebuggerPlugin *m_plugin;
    clTreeListCtrl* m_treeList;
    LLDBLocalsView::IntItemMap_t m_pendingExpandItems;
    
private:
    void DoAddVariableToView(const LLDBLocalVariable::Vect_t& variables, wxTreeItemId parent);
    LLDBLocalVariableClientData *GetItemData(const wxTreeItemId &id);
    void Cleanup();
    
protected:
    // events coming from LLDB
    void OnLLDBStarted(LLDBEvent &event);
    void OnLLDBExited(LLDBEvent &event);
    void OnLLDBLocalsUpdated(LLDBEvent &event);
    void OnLLDBRunning(LLDBEvent &event);
    void OnLLDBVariableExpanded(LLDBEvent &event);
    
    // UI events
    void OnItemExpanding(wxTreeEvent &event);
    
public:
    LLDBLocalsView(wxWindow* parent, LLDBDebuggerPlugin* plugin);
    virtual ~LLDBLocalsView();
};
#endif // LLDBLOCALSVIEW_H
