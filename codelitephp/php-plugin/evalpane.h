#ifndef EVALPANE_H
#define EVALPANE_H
#include "php_ui.h"
#include "xdebugevent.h"

class EvalPane : public EvalPaneBase
{
public:
    EvalPane(wxWindow* parent);
    virtual ~EvalPane();
protected:
    virtual void OnSendXDebugCommand(wxCommandEvent& event);
    virtual void OnSendXDebugCommandUI(wxUpdateUIEvent& event);
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnSend(wxCommandEvent& event);
    virtual void OnSendUI(wxUpdateUIEvent& event);
    void OnExpressionEvaluate(XDebugEvent &e);
    void OnDBGPCommandEvaluated(XDebugEvent &e);

};
#endif // EVALPANE_H
