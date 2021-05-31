//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : procutils.h
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
#ifndef PROCUTILS_H
#define PROCUTILS_H

#include "codelite_exports.h"
#include <map>
#include <vector>
#include <wx/arrstr.h>
#include <wx/process.h>
#include <wx/string.h>

#include "wx/defs.h"
#ifdef __WINDOWS__          // __WINDOWS__ defined by wx/defs.h
#include "wx/msw/wrapwin.h" // includes windows.h
//#include <devpropdef.h>
#include <Psapi.h>
#include <SetupAPI.h>
#include <tlhelp32.h>
#endif

struct ProcessEntry {
    wxString name;
    long pid;
};

typedef std::vector<ProcessEntry> PidVec_t;

class WXDLLIMPEXP_CL ProcUtils
{
public:
    ProcUtils();
    ~ProcUtils();

    static void GetProcTree(std::map<unsigned long, bool>& parentsMap, long pid);
    static void ExecuteCommand(const wxString& command, wxArrayString& output,
                               long flags = wxEXEC_NODISABLE | wxEXEC_SYNC);
    static void ExecuteInteractiveCommand(const wxString& command);
    static wxString GetProcessNameByPid(long pid);
    static void GetProcessList(std::vector<ProcessEntry>& proclist);
    static void GetChildren(long pid, std::vector<long>& children);
    static bool Shell(const wxString& programConsoleCommand);
    static bool Locate(const wxString& name, wxString& where);

    /**
     * @brief the equivalent of 'ps ax|grep <name>'
     */
    static PidVec_t PS(const wxString& name);

    /**
     * \brief a safe function that executes 'command' and returns its output. This function
     * is safed to be called from secondary thread (hence, SafeExecuteCommand)
     * \param command
     * \param output
     */
    static void SafeExecuteCommand(const wxString& command, wxArrayString& output);

    /**
     * @brief execute a command and return its output as plain string
     */
    static wxString SafeExecuteCommand(const wxString& command);
};

#endif // PROCUTILS_H
