#include "NodeJSCliDebuggerPane.h"
#include "NodeJSEvents.h"
#include "event_notifier.h"

NodeJSCliDebuggerPane::NodeJSCliDebuggerPane(wxWindow* parent)
    : NodeJSCliDebuggerPaneBase(parent)
{
    EventNotifier::Get()->Bind(wxEVT_NODEJS_CLI_DEBUGGER_UPDATE_CALLSTACK, &NodeJSCliDebuggerPane::OnUpdateBacktrace,
                               this);
}

NodeJSCliDebuggerPane::~NodeJSCliDebuggerPane()
{
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_CLI_DEBUGGER_UPDATE_CALLSTACK, &NodeJSCliDebuggerPane::OnUpdateBacktrace,
                                 this);
}

void NodeJSCliDebuggerPane::OnUpdateBacktrace(clDebugEvent& event)
{
    event.Skip();
    wxString bt = event.GetString();
    wxArrayString arr = ::wxStringTokenize(bt, "\n", wxTOKEN_STRTOK);
    for(size_t i = 0; i < arr.size(); ++i) {
        const wxString& frame = arr.Item(i).Trim().Trim(false);
        if(!frame.StartsWith("#")) { continue; }
        
    }
}
