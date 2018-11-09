#ifndef DEBUGGERPANE_H
#define DEBUGGERPANE_H
#include "WebToolsBase.h"

class DebuggerPane : public NodeJSCliDebuggerPaneBase
{
public:
    DebuggerPane(wxWindow* parent);
    virtual ~DebuggerPane();
};
#endif // DEBUGGERPANE_H
