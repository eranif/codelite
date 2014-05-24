#include "CompilersDetectorManager.h"
#include "CompilerLocatorMinGW.h"
#include "CompilerLocatorGCC.h"

CompilersDetectorManager::CompilersDetectorManager()
{
#ifdef __WXMSW__
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorMinGW()) );
    
#elif defined(__WXGTK__)
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorGCC()) );
    
#elif defined(__WXMAC__)
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorGCC()) );
    
#endif
}

CompilersDetectorManager::~CompilersDetectorManager()
{
}

bool CompilersDetectorManager::Locate()
{
    m_compilersFound.clear();
    ICompilerLocator::Vect_t::iterator iter = m_detectors.begin();
    for(; iter != m_detectors.end(); ++iter ) {
        if ( (*iter)->Locate() ) {
            m_compilersFound.insert( m_compilersFound.end(), (*iter)->GetCompilers().begin(), (*iter)->GetCompilers().end()) ;
        }
    }
    return !m_compilersFound.empty();
}
