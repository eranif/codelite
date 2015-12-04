//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ZombieReaperPOSIX.h
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

#ifndef ZOMBIEREAPERPOSIX_H
#define ZOMBIEREAPERPOSIX_H

#ifndef __WXMSW__

#include <wx/event.h>
#include <wx/thread.h>
#include "codelite_exports.h"
#include <wx/process.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CL_PROCESS_TERMINATED, wxProcessEvent);

class WXDLLIMPEXP_CL ZombieReaperPOSIX : public wxThread
{
public:
    virtual void* Entry();
    ZombieReaperPOSIX();
    virtual ~ZombieReaperPOSIX();

    void Start() {
        Create();
        Run();
    }
    
    void Stop();
};
#endif 
#endif // ZOMBIEREAPERPOSIX_H

