//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : cl_aui_notebook_art.h
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

#ifndef CLAUINOTEBOOKTABART_H
#define CLAUINOTEBOOKTABART_H

#include "codelite_exports.h"
#include "clAuiMainNotebookTabArt.h"

class WXDLLIMPEXP_SDK clAuiGlossyTabArt : public clAuiMainNotebookTabArt
{
protected:
    virtual void DoSetColours();

public:
    clAuiGlossyTabArt();
    virtual ~clAuiGlossyTabArt();
    virtual wxAuiTabArt* Clone() { return new clAuiGlossyTabArt(*this); }
};

#endif // CLAUINOTEBOOKTABART_H
