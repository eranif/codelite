#include "CompilersDetectorManager.h"
#include "CompilerLocatorMinGW.h"
#include "CompilerLocatorGCC.h"
#include "CompilerLocatorMSVC9.h"   // VS 2008
#include "CompilerLocatorMSVC10.h"  // VS 2010
#include "CompilerLocatorMSVC11.h"  // VS 2012
#include "CompilerLocatorMSVC12.h"  // VS 2013
#include "CompilerLocatorCLANG.h"

CompilersDetectorManager::CompilersDetectorManager()
{
#ifdef __WXMSW__
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorMinGW()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorCLANG()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorMSVC9()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorMSVC10()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorMSVC11()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorMSVC12()) );
    
#elif defined(__WXGTK__)
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorGCC()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorCLANG()) );
    
#elif defined(__WXMAC__)
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorGCC()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorCLANG()) );
    
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
