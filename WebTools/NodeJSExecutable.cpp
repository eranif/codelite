#include "NodeJSExecutable.h"
#include "asyncprocess.h"
#include "globals.h"
#include "WebToolsConfig.h"

#define DEFAULT_VERSION 4

NodeJSExecutable::NodeJSExecutable(const wxFileName& exePath)
    : m_exePath(exePath)
{
}

NodeJSExecutable::NodeJSExecutable()
{
    WebToolsConfig conf;
    conf.Load();
    m_exePath = conf.GetNodejs();
}

NodeJSExecutable::~NodeJSExecutable() {}

bool NodeJSExecutable::Exists() const { return m_exePath.FileExists(); }

int NodeJSExecutable::GetMajorVersion() const
{
    if(!Exists()) { return DEFAULT_VERSION; }
    wxString command;
    wxString version_output;
    command << m_exePath.GetFullPath();
    ::WrapWithQuotes(command);
    command << " -v";
    IProcess::Ptr_t cmd(::CreateSyncProcess(command, IProcessCreateDefault));
    cmd->WaitForTerminate(version_output);
    if(version_output.IsEmpty()) { return DEFAULT_VERSION; }
    version_output.StartsWith("v", &version_output);
    version_output = version_output.BeforeFirst('.');
    long nMajorVersion = DEFAULT_VERSION;
    if(version_output.ToCLong(&nMajorVersion)) { return nMajorVersion; }
    return DEFAULT_VERSION;
}
