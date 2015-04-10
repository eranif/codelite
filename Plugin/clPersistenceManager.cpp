#include "clPersistenceManager.h"

clPersistenceManager::clPersistenceManager()
    : m_iniFile(new clIniFile("persistency.ini")) // ~/.codelite/config/persistency.ini"
{
}

clPersistenceManager::~clPersistenceManager() { wxDELETE(m_iniFile); }

wxConfigBase* clPersistenceManager::GetConfig() const { return m_iniFile; }
