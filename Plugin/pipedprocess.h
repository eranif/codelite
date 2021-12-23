//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : pipedprocess.h
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
#ifndef PipedProcess_H
#define PipedProcess_H

#include "codelite_exports.h"
#include "wx/process.h"
#include "wx/string.h"

class WXDLLIMPEXP_SDK PipedProcess : public wxProcess
{
    long m_pid;
    wxString m_cmd;

public:
    /**
     * Constructs a process object. id is only used in the case you want to use wxWidgets events.
     * It identifies this object, or another window that will receive the event.
     */
    PipedProcess(int id, const wxString& cmdLine);

    virtual ~PipedProcess();

    /**
     * \return return the process id
     */
    long GetPid();

    /**
     * \param set the process ID
     */
    void SetPid(long pid);

    /**
     * Kill the process
     */
    void Terminate();

    /**
     * Start the process
     * \return the process id
     */
    virtual long Start(bool hide = true);

    virtual bool HasInput(wxString& input);

    /**
	 * \brief read all input from the process output stream
	 * \param input
	 * \return true if some data was
     * read, false otherwise
	 */
    virtual bool ReadAll(wxString& input);
};

#endif // PipedProcess_H
