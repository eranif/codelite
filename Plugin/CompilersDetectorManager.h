#ifndef COMPILERSDETECTORMANAGER_H
#define COMPILERSDETECTORMANAGER_H

#include <ICompilerLocator.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK CompilersDetectorManager
{
    ICompilerLocator::Vect_t m_detectors;
    ICompilerLocator::CompilerVec_t m_compilersFound;

public:
    CompilersDetectorManager();
    virtual ~CompilersDetectorManager();

    void SetCompilersFound(const ICompilerLocator::CompilerVec_t& compilersFound) {
        this->m_compilersFound = compilersFound;
    }
    void SetDetectors(const ICompilerLocator::Vect_t& detectors) {
        this->m_detectors = detectors;
    }
    const ICompilerLocator::CompilerVec_t& GetCompilersFound() const {
        return m_compilersFound;
    }
    const ICompilerLocator::Vect_t& GetDetectors() const {
        return m_detectors;
    }
    bool Locate();
};

#endif // COMPILERSDETECTORMANAGER_H
