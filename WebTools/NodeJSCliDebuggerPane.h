#ifndef NODEJSCLIDEBUGGERPANE_H
#define NODEJSCLIDEBUGGERPANE_H
#include "WebToolsBase.h"
#include "cl_command_event.h"

class NodeJSCliDebuggerPane : public NodeJSCliDebuggerPaneBase
{
protected:
    void OnUpdateBacktrace(clDebugEvent& event);

public:
    NodeJSCliDebuggerPane(wxWindow* parent);
    virtual ~NodeJSCliDebuggerPane();
};
#endif // NODEJSCLIDEBUGGERPANE_H
