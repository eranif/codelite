#include "DebuggerScriptParsed.h"
#include "NodeFileManager.h"

DebuggerScriptParsed::DebuggerScriptParsed()
    : NodeMessageBase("Debugger.scriptParsed")
{
}

DebuggerScriptParsed::~DebuggerScriptParsed() {}

void DebuggerScriptParsed::Process(const JSONElement& json)
{
    wxString scriptId = json.namedObject("scriptId").toString();
    wxString url = json.namedObject("url").toString();
    NodeFileManager::Get().AddFile(scriptId, url);
}

NodeMessageBase::Ptr_t DebuggerScriptParsed::Clone() { return NodeMessageBase::Ptr_t(new DebuggerScriptParsed()); }
