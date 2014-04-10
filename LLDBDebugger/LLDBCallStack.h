#ifndef LLDBCALLSTACK_H
#define LLDBCALLSTACK_H

#include "UI.h"
#include "LLDBProtocol/LLDBBacktrace.h"
#include "LLDBProtocol/LLDBEvent.h"

class LLDBDebugger;
class LLDBCallStackPane : public LLDBCallStackBase
{
    LLDBDebugger* m_lldb;
protected:
    void OnBacktrace(LLDBEvent &event);
    void OnRunning(LLDBEvent &event);
    
public:
    LLDBCallStackPane(wxWindow* parent, LLDBDebugger* lldb);
    virtual ~LLDBCallStackPane();
};
#endif // LLDBCALLSTACK_H
