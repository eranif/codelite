//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPSymbolsCacher.h
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

#ifndef PHPSYMBOLSCACHER_H
#define PHPSYMBOLSCACHER_H

#include "job.h" // Base class: Job
#include <wx/string.h>
#include <wx/event.h>

class PHPCodeCompletion;
class PHPSymbolsCacher : public Job
{
    PHPCodeCompletion* m_owner;
    wxString m_filename;
    
public:
    PHPSymbolsCacher(PHPCodeCompletion* owner, const wxString& dbfile);
    virtual ~PHPSymbolsCacher();

public:
    virtual void Process(wxThread* thread);
};

#endif // PHPSYMBOLSCACHER_H
