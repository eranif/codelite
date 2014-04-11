#ifndef LLDBCALLSTACK_H
#define LLDBCALLSTACK_H

#include "UI.h"
#include "LLDBProtocol/LLDBBacktrace.h"
#include "LLDBProtocol/LLDBEvent.h"

class LLDBConnector;
class LLDBCallStackPane : public LLDBCallStackBase
{
    LLDBConnector* m_connector;
protected:
    void OnBacktrace(LLDBEvent &event);
    void OnRunning(LLDBEvent &event);
    
public:
    LLDBCallStackPane(wxWindow* parent, LLDBConnector* connector);
    virtual ~LLDBCallStackPane();
};
#endif // LLDBCALLSTACK_H
