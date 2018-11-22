#include "NodeJSWorkspaceUserConfiguration.h"
#include <globals.h>
#include <json_node.h>

NodeJSWorkspaceUser::NodeJSWorkspaceUser(const wxString& workspacePath)
    : m_workspacePath(workspacePath)
    , m_debuggerPort(5858)
    , m_debuggerHost("127.0.0.1")
{
}

NodeJSWorkspaceUser::~NodeJSWorkspaceUser() {}

wxFileName NodeJSWorkspaceUser::GetFileName() const
{
    wxFileName workspaceFile(m_workspacePath);
    wxFileName fn(workspaceFile.GetPath(), workspaceFile.GetFullName() + ".nodejs." + clGetUserName());
    fn.AppendDir(".codelite");
    if(!fn.FileExists()) {
        fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
    return fn;
}

NodeJSWorkspaceUser& NodeJSWorkspaceUser::Load()
{
    wxFileName fn = GetFileName();
    JSONRoot root(fn);
    JSONElement element = root.toElement();

    m_debuggerPort = element.namedObject("m_debuggerPort").toInt(m_debuggerPort);
    m_debuggerHost = element.namedObject("m_debuggerHost").toString(m_debuggerHost);
    m_scriptToExecute = element.namedObject("m_scriptToExecute").toString(m_scriptToExecute);
    m_commandLineArgs = element.namedObject("m_commandLineArgs").toArrayString();
    m_workingDirectory = element.namedObject("m_workingDirectory").toString();

    m_breakpoints.clear();
    JSONElement bpArr = element.namedObject("m_breakpoints");
    int bpcount = bpArr.arraySize();
    for(int i = 0; i < bpcount; ++i) {
        NodeJSBreakpoint bp;
        bp.FromJSON(bpArr.arrayItem(i));
        m_breakpoints.push_back(bp);
    }
    return *this;
}

NodeJSWorkspaceUser& NodeJSWorkspaceUser::Save()
{
    // Serialize the breakpoints
    JSONRoot root(cJSON_Object);
    JSONElement json = root.toElement();
    json.addProperty("m_debuggerPort", m_debuggerPort);
    json.addProperty("m_debuggerHost", m_debuggerHost);
    json.addProperty("m_scriptToExecute", m_scriptToExecute);
    json.addProperty("m_commandLineArgs", m_commandLineArgs);
    json.addProperty("m_workingDirectory", m_workingDirectory);
    JSONElement bpArr = JSONElement::createArray("m_breakpoints");
    json.append(bpArr);

    NodeJSBreakpoint::Vec_t::const_iterator iter = m_breakpoints.begin();
    for(; iter != m_breakpoints.end(); ++iter) {
        bpArr.arrayAppend(iter->ToJSON(""));
    }
    root.save(GetFileName());
    return *this;
}
