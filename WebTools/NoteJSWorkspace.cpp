#include "NoteJSWorkspace.h"
NodeJSWorkspace* NodeJSWorkspace::ms_workspace = NULL;

NodeJSWorkspace::NodeJSWorkspace() { SetWorkspaceType("Node.js"); }

NodeJSWorkspace::~NodeJSWorkspace() {}

bool NodeJSWorkspace::IsBuildSupported() const { return false; }
bool NodeJSWorkspace::IsProjectSupported() const { return false; }

void NodeJSWorkspace::Free()
{
    if(ms_workspace) {
        delete ms_workspace;
    }
    ms_workspace = NULL;
}

NodeJSWorkspace* NodeJSWorkspace::Get()
{
    if(!ms_workspace) {
        ms_workspace = new NodeJSWorkspace();
    }
    return ms_workspace;
}
