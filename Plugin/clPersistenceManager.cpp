#include "clPersistenceManager.h"

#include "cl_standard_paths.h"

clPersistenceManager::clPersistenceManager()
    : m_fileConfig(std::make_unique<wxFileConfig>(
          "",
          "",
          clStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "config" +
              wxFileName::GetPathSeparator() + "persistency.ini", // ~/.codelite/config/persistency.ini"
          "",
          wxCONFIG_USE_LOCAL_FILE))
{
}

clPersistenceManager::~clPersistenceManager() { m_fileConfig->Flush(); }

wxConfigBase* clPersistenceManager::GetConfig() const { return m_fileConfig.get(); }
