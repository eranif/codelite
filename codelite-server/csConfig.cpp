#include "clIniFile.h"
#include "cl_standard_paths.h"
#include "csConfig.h"
#include "file_logger.h"
#include <wx/filename.h>

csConfig::csConfig() {}

csConfig::~csConfig() {}

void csConfig::Load()
{
    wxFileName iniPath(clStandardPaths::Get().GetUserDataDir(), "codelite-server.ini");
    clIniFile ini(iniPath);
    ini.Read("connection_string", &m_connectionString, "tcp://127.0.0.1:5555");
    clDEBUG() << "connection_string =" << m_connectionString;
}
