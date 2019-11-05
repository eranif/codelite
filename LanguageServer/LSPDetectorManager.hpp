#ifndef LSPDETECTORMANAGER_HPP
#define LSPDETECTORMANAGER_HPP

#include "LSPDetector.hpp"
#include <vector>

class LSPDetectorManager
{
    std::vector<LSPDetector::Ptr_t> m_detectors;

public:
    LSPDetectorManager();
    virtual ~LSPDetectorManager();

    static LSPDetectorManager& Get();

    /**
     * @brief scan for language servers installed on this machine
     */
    bool Scan();
};

#endif // LSPDETECTORMANAGER_HPP
