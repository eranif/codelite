//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPLint.h
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

#ifndef PHPLINT_H
#define PHPLINT_H

#include <wx/string.h>
#include <wx/event.h>
#include <list>
#include <wx/filename.h>
#include "smart_ptr.h"
#include "cl_command_event.h"

class PhpPlugin;
class IProcess;
class PHPLint : public wxEvtHandler
{
    PhpPlugin* m_plugin;
    std::list<wxFileName> m_queue;
    wxString m_output;
    IProcess* m_process;
    wxString m_currentFileBeingProcessed;
    
public:
    typedef SmartPtr<PHPLint> Ptr_t;
    
protected:
    void DoProcessQueue();
    void DoCheckFile(const wxFileName& filename);
    
public:
    PHPLint(PhpPlugin *plugin);
    ~PHPLint();
    
    /**
     * @brief run php line on the current file and report back the error
     */
    void CheckCode(const wxFileName& filename);
    
    void OnProcessTerminated(clProcessEvent &event);
    void OnProcessOutput(clProcessEvent &event);
};

#endif // PHPLINT_H
