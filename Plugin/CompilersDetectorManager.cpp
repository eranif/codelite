//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CompilersDetectorManager.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "CompilersDetectorManager.h"
#include "CompilerLocatorMinGW.h"
#include "CompilerLocatorGCC.h"
#include "CompilerLocatorCrossGCC.h"
#include "CompilerLocatorMSVC9.h"   // VS 2008
#include "CompilerLocatorMSVC10.h"  // VS 2010
#include "CompilerLocatorMSVC11.h"  // VS 2012
#include "CompilerLocatorMSVC12.h"  // VS 2013
#include "CompilerLocatorCLANG.h"
#include "CompilerLocatorCygwin.h"

CompilersDetectorManager::CompilersDetectorManager()
{
#ifdef __WXMSW__
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorMinGW()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorCLANG()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorMSVC9()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorMSVC10()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorMSVC11()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorMSVC12()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorCygwin()) );
    
#elif defined(__WXGTK__)
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorGCC()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorCLANG()) );
    
#elif defined(__WXMAC__)
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorGCC()) );
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorCLANG()) );
    
#endif
    m_detectors.push_back( ICompilerLocator::Ptr_t(new CompilerLocatorCrossGCC()) );
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

CompilerPtr CompilersDetectorManager::Locate(const wxString& folder)
{
    m_compilersFound.clear();
    ICompilerLocator::Vect_t::iterator iter = m_detectors.begin();
    for(; iter != m_detectors.end(); ++iter ) {
        CompilerPtr comp = (*iter)->Locate( folder );
        if ( comp ) {
            return comp;
        }
    }
    return NULL;
}
