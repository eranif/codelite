//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CompilerLocatorMSVC9.h
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

#ifndef COMPILERLOCATORMSVC9_H
#define COMPILERLOCATORMSVC9_H

#include "CompilerLocatorMSVCBase.h" // Base class: CompilerLocatorMSVCBase

/// Visual Studio 2008 (VC9)
class CompilerLocatorMSVC9 : public CompilerLocatorMSVCBase
{
public:
    CompilerLocatorMSVC9();
    virtual ~CompilerLocatorMSVC9();
    
    virtual bool Locate();
};

#endif // COMPILERLOCATORMSVC9_H
