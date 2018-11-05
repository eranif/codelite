#ifndef NODEJSCLIDEBUGGERPANE_H
#define NODEJSCLIDEBUGGERPANE_H

#include "NodeJSDebugEvent.h"
#include "WebToolsBase.h"
#include "cl_command_event.h"

class DebuggerPane : public NodeJSCliDebuggerPaneBase
{
protected:
    void OnUpdateBacktrace(NodeJSDebugEvent& event);
    void OnDebuggerStopped(clDebugEvent& event);
    void OnMarkLine(clDebugEvent& event);

public:
    DebuggerPane(wxWindow* parent);
    virtual ~DebuggerPane();
};
#endif // NODEJSCLIDEBUGGERPANE_H
