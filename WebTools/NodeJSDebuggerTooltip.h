#ifndef NODEJSDEBUGGERTOOLTIP_H
#define NODEJSDEBUGGERTOOLTIP_H
#include "WebToolsBase.h"

class NodeJSDebuggerTooltip : public NodeJSDebuggerTooltipBase
{
public:
    NodeJSDebuggerTooltip(wxWindow* parent);
    virtual ~NodeJSDebuggerTooltip();
protected:
    virtual void OnItemExpanding(wxTreeEvent& event);
};
#endif // NODEJSDEBUGGERTOOLTIP_H
