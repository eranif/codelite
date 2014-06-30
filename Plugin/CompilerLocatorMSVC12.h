//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CompilerLocatorMSVC12.h
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

#ifndef COMPILERLOCATORMSVC12_H
#define COMPILERLOCATORMSVC12_H

#include "CompilerLocatorMSVCBase.h" // Base class: CompilerLocatorMSVCBase

class CompilerLocatorMSVC12 : public CompilerLocatorMSVCBase
{
public:
    CompilerLocatorMSVC12();
    virtual ~CompilerLocatorMSVC12();
    
    virtual bool Locate();
};

#endif // COMPILERLOCATORMSVC12_H
