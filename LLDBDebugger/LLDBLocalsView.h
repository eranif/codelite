#ifndef LLDBLOCALSVIEW_H
#define LLDBLOCALSVIEW_H

#include "UI.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBLocalVariable.h"

class LLDBDebuggerPlugin;
class LLDBLocalsView : public LLDBLocalsViewBase
{
    LLDBDebuggerPlugin *m_plugin;
    
private:
    void DoAddVariableToView(LLDBLocalVariable::Ptr_t variable, const wxDataViewItem &parent);
    
protected:
    void OnLLDBStarted(LLDBEvent &event);
    void OnLLDBExited(LLDBEvent &event);
    void OnLLDBStopped(LLDBEvent &event);
    void OnLLDBRunning(LLDBEvent &event);
    
public:
    LLDBLocalsView(wxWindow* parent, LLDBDebuggerPlugin* plugin);
    virtual ~LLDBLocalsView();
};
#endif // LLDBLOCALSVIEW_H
