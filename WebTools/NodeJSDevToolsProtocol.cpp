#include "NodeJSDevToolsProtocol.h"
#include "json_node.h"

NodeJSDevToolsProtocol::NodeJSDevToolsProtocol() {}

NodeJSDevToolsProtocol::~NodeJSDevToolsProtocol() {}

wxArrayString NodeJSDevToolsProtocol::GetStartupCommands()
{
    wxArrayString commands;
    {
        JSONRoot root(cJSON_Object);
        JSONElement e = root.toElement();
        e.addProperty("id", ++message_id);
        e.addProperty("method", "Runtime.enable");

        commands.Add(e.format(false));
    }
    {
        JSONRoot root(cJSON_Object);
        JSONElement e = root.toElement();
        e.addProperty("id", ++message_id);
        e.addProperty("method", "Debugger.enable");

        commands.Add(e.format(false));
    }
    {
        JSONRoot root(cJSON_Object);
        JSONElement e = root.toElement();
        e.addProperty("id", ++message_id);
        e.addProperty("method", "Runtime.runIfWaitingForDebugger");

        commands.Add(e.format(false));
    }
    return commands;
}
