//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CompilerLocatorMSVC14.h
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

#ifndef COMPILERLOCATORMSVC14_H
#define COMPILERLOCATORMSVC14_H

#include "CompilerLocatorMSVCBase.h" // Base class: CompilerLocatorMSVCBase

// Visual C++ 14 (aka Visual Studio 2015)
class CompilerLocatorMSVC14 : public CompilerLocatorMSVCBase
{
public:
    CompilerLocatorMSVC14();
    virtual ~CompilerLocatorMSVC14();

    virtual bool Locate();
};

#endif // COMPILERLOCATORMSVC14_H
