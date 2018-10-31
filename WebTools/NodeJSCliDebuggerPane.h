#ifndef NODEJSCLIDEBUGGERPANE_H
#define NODEJSCLIDEBUGGERPANE_H
#include "WebToolsBase.h"
#include "cl_command_event.h"

class NodeJSCliDebuggerPane : public NodeJSCliDebuggerPaneBase
{
protected:
    void OnUpdateBacktrace(clDebugEvent& event);
    void OnDebuggerStopped(clDebugEvent& event);
    void OnMarkLine(clDebugEvent& event);

public:
    NodeJSCliDebuggerPane(wxWindow* parent);
    virtual ~NodeJSCliDebuggerPane();
};
#endif // NODEJSCLIDEBUGGERPANE_H
