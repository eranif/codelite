#include "clIniFile.h"
#include "cl_standard_paths.h"
#include "csConfig.h"
#include "file_logger.h"
#include <wx/filename.h>

csConfig::csConfig()
    : m_flags(0)
{
}

csConfig::~csConfig() {}

void csConfig::Load()
{
    wxFileName iniPath(clStandardPaths::Get().GetUserDataDir(), "codelite-cli.ini");
    clIniFile ini(iniPath);
    ini.Read("command", &m_command, "");
    clDEBUG() << "command =" << m_command;
    ini.Read("options", &m_options, "");
    clDEBUG() << "options =" << m_options;
    bool pretty_json = false;
    ini.Read("pretty_json", &pretty_json);
    EnableFlag(kPrettyJSON, pretty_json);
}
