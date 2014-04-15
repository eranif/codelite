#ifndef LLDBCALLSTACK_H
#define LLDBCALLSTACK_H

#include "UI.h"
#include "LLDBProtocol/LLDBBacktrace.h"
#include "LLDBProtocol/LLDBEvent.h"

class LLDBConnector;
class LLDBCallStackPane : public LLDBCallStackBase
{
    LLDBConnector* m_connector;
    int m_selectedFrame;
protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
    void OnBacktrace(LLDBEvent &event);
    void OnRunning(LLDBEvent &event);

public:
    LLDBCallStackPane(wxWindow* parent, LLDBConnector* connector);
    virtual ~LLDBCallStackPane();
    
    void SetSelectedFrame(int selectedFrame) {
        this->m_selectedFrame = selectedFrame;
    }
    int GetSelectedFrame() const {
        return m_selectedFrame;
    }
};
#endif // LLDBCALLSTACK_H
