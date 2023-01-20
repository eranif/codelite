#include "LSPDetectorManager.hpp"

#include "LanguageServerConfig.h"
#include "LanguageServerEntry.h"
#include "detectors/LSPCMakeDetector.hpp"
#include "detectors/LSPCTagsdDetector.hpp"
#include "detectors/LSPClangdDetector.hpp"
#include "detectors/LSPPythonDetector.hpp"
#include "detectors/LSPRustAnalyzerDetector.hpp"
#include "detectors/LSPTypeScriptDetector.hpp"
#include "environmentconfig.h"
#include "file_logger.h"

LSPDetectorManager::LSPDetectorManager()
{
    m_detectors.push_back(LSPDetector::Ptr_t(new LSPClangdDetector()));
    m_detectors.push_back(LSPDetector::Ptr_t(new LSPPythonDetector()));
    m_detectors.push_back(LSPDetector::Ptr_t(new LSPRustAnalyzerDetector()));
    m_detectors.push_back(LSPDetector::Ptr_t(new LSPTypeScriptDetector()));
    m_detectors.push_back(LSPDetector::Ptr_t(new LSPCTagsdDetector()));
    m_detectors.push_back(LSPDetector::Ptr_t(new LSPCMakeDetector()));
}

LSPDetectorManager::~LSPDetectorManager() {}

size_t LSPDetectorManager::Scan(std::vector<LSPDetector::Ptr_t>& matchers)
{
    // apply the environment before searching for the binary
    EnvSetter env;

    for(LSPDetector::Ptr_t detector : m_detectors) {
        LSP_DEBUG() << "LSP detector: trying" << detector->GetName();
        if(detector->Locate()) {
            LSP_DEBUG() << "  ==> " << detector->GetName() << "found";
            matchers.push_back(detector);
        }
    }
    return matchers.size();
}
