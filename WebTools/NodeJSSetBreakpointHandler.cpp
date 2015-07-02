#include "NodeJSSetBreakpointHandler.h"
#include "NodeJSDebugger.h"
#include "json_node.h"
#include "file_logger.h"

NodeJSSetBreakpointHandler::NodeJSSetBreakpointHandler(const NodeJSBreakpoint& bp)
    : m_breakpoint(bp)
{
}

NodeJSSetBreakpointHandler::~NodeJSSetBreakpointHandler() {}
void NodeJSSetBreakpointHandler::Process(NodeJSDebugger* debugger, const wxString& output)
{
    JSONRoot root(output);
    JSONElement json = root.toElement();
    int bpId = json.namedObject("body").namedObject("breakpoint").toInt();
    NodeJSBreakpoint& bp =
        debugger->GetBreakpointsMgr()->GetBreakpoint(m_breakpoint.GetFilename(), m_breakpoint.GetLine());
    if(bp.IsOk()) {
        bp.SetNodeBpID(bpId);
        CL_DEBUG("CodeLite: Found breakpoint ID: %d", bpId);
    }
}
