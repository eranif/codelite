//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CompilerLocatorMSVC11.h
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

#ifndef COMPILERLOCATORMSVC11_H
#define COMPILERLOCATORMSVC11_H

#include "CompilerLocatorMSVCBase.h" // Base class: CompilerLocatorMSVCBase

class CompilerLocatorMSVC11 : public CompilerLocatorMSVCBase
{
public:
    CompilerLocatorMSVC11();
    virtual ~CompilerLocatorMSVC11();
    
    virtual bool Locate();
};

#endif // COMPILERLOCATORMSVC11_H
