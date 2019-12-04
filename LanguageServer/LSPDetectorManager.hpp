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

    /**
     * @brief scan for language servers installed on this machine
     */
    size_t Scan(std::vector<LSPDetector::Ptr_t>& matchers);
};

#endif // LSPDETECTORMANAGER_HPP
