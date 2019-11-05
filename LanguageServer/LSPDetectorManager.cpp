#include "LSPClangdDetector.hpp"
#include "LSPDetectorManager.hpp"
#include "LanguageServerConfig.h"
#include "LanguageServerEntry.h"

LSPDetectorManager::LSPDetectorManager() { m_detectors.push_back(LSPDetector::Ptr_t(new LSPClangdDetector())); }

LSPDetectorManager::~LSPDetectorManager() {}

LSPDetectorManager& LSPDetectorManager::Get()
{
    static LSPDetectorManager manager;
    return manager;
}

bool LSPDetectorManager::Scan()
{
    bool foundSomething = false;
    for(LSPDetector::Ptr_t detector : m_detectors) {
        if(detector->Locate()) {
            foundSomething = true;
            // add new LSP
            LanguageServerConfig& conf = LanguageServerConfig::Get();
            LanguageServerEntry entry;
            entry.SetLanguages(detector->GetLangugaes());
            entry.SetCommand(detector->GetCommand());
            entry.SetEnabled(true);
            entry.SetDisaplayDiagnostics(true);
            entry.SetConnectionString(detector->GetConnectionString());
            entry.SetPriority(detector->GetPriority());
            conf.AddServer(entry);
            conf.Save();
        }
    }
    return foundSomething;
}
