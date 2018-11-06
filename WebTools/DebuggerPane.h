#ifndef NODEJSCLIDEBUGGERPANE_H
#define NODEJSCLIDEBUGGERPANE_H

#include "clDebugCallFramesEvent.h"
#include "WebToolsBase.h"
#include "cl_command_event.h"

class DebuggerPane : public NodeJSCliDebuggerPaneBase
{
protected:
    void OnUpdateBacktrace(clDebugCallFramesEvent& event);
    void OnDebuggerStopped(clDebugEvent& event);
    void OnMarkLine(clDebugEvent& event);
    void OnInteract(clDebugEvent& event);

public:
    DebuggerPane(wxWindow* parent);
    virtual ~DebuggerPane();
};
#endif // NODEJSCLIDEBUGGERPANE_H
