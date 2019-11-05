#include "LSPClangdDetector.hpp"
#include "LSPDetectorManager.hpp"
#include "LanguageServerConfig.h"
#include "LanguageServerEntry.h"
#include "file_logger.h"

LSPDetectorManager::LSPDetectorManager() { m_detectors.push_back(LSPDetector::Ptr_t(new LSPClangdDetector())); }

LSPDetectorManager::~LSPDetectorManager() {}

LSPDetectorManager& LSPDetectorManager::Get()
{
    static LSPDetectorManager manager;
    return manager;
}

size_t LSPDetectorManager::Scan(std::vector<LSPDetector::Ptr_t>& matchers)
{
    for(LSPDetector::Ptr_t detector : m_detectors) {
        clDEBUG() << "LSP detector: trying" << detector->GetName();
        if(detector->Locate()) {
            clDEBUG() << "  ==> " << detector->GetName() << "found";
            matchers.push_back(detector);
        }
    }
    return matchers.size();
}
