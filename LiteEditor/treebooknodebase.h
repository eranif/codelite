//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : treebooknodebase.h
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

#ifndef __treebooknodebase__
#define __treebooknodebase__

#include "editor_config.h"

class TreeBookNodeBase
{
public:
    TreeBookNodeBase();
    virtual ~TreeBookNodeBase();

    virtual void Save(OptionsConfigPtr options) = 0;
    virtual bool IsRestartRequired() { return false; }
};

template <typename DerivedT> class TreeBookNode : public TreeBookNodeBase
{

public:
    TreeBookNode()
        : TreeBookNodeBase()
    {
    }

    virtual ~TreeBookNode() {}
};
#endif // __treebooknodebase__
