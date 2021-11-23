#include "LSPDetectorManager.hpp"
#include "LSPCTagsdDetector.hpp"
#include "LSPClangdDetector.hpp"
#include "LSPPythonDetector.hpp"
#include "LSPRlsDetector.hpp"
#include "LSPRustAnalyzerDetector.hpp"
#include "LSPTypeScriptDetector.hpp"
#include "LanguageServerConfig.h"
#include "LanguageServerEntry.h"
#include "environmentconfig.h"
#include "file_logger.h"

LSPDetectorManager::LSPDetectorManager()
{
    m_detectors.push_back(LSPDetector::Ptr_t(new LSPClangdDetector()));
    m_detectors.push_back(LSPDetector::Ptr_t(new LSPPythonDetector()));
    m_detectors.push_back(LSPDetector::Ptr_t(new LSPRlsDetector()));
    m_detectors.push_back(LSPDetector::Ptr_t(new LSPRustAnalyzerDetector()));
    m_detectors.push_back(LSPDetector::Ptr_t(new LSPTypeScriptDetector()));
    m_detectors.push_back(LSPDetector::Ptr_t(new LSPCTagsdDetector()));
}

LSPDetectorManager::~LSPDetectorManager() {}

size_t LSPDetectorManager::Scan(std::vector<LSPDetector::Ptr_t>& matchers)
{
    // apply the environment before searching for the binary
    EnvSetter env;

    for(LSPDetector::Ptr_t detector : m_detectors) {
        clDEBUG() << "LSP detector: trying" << detector->GetName();
        if(detector->Locate()) {
            clDEBUG() << "  ==> " << detector->GetName() << "found";
            matchers.push_back(detector);
        }
    }
    return matchers.size();
}
