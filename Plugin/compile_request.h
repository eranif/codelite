//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : compile_request.h
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
#ifndef COMPILE_REQUEST_H
#define COMPILE_REQUEST_H

#include "codelite_exports.h"
#include "shell_command.h"

class WXDLLIMPEXP_SDK CompileRequest : public ShellCommand
{
    wxString m_fileName;
    bool m_premakeOnly;
    bool m_preprocessOnly;

public:
    /**
     * Construct a compilation request. The compiler thread will build the selected project and all
     * its dependencies as appeard in the build order dialog
     * \param projectName the selected project to build
     * \param configurationName the workspace selected configuration
     */
    CompileRequest(const QueueCommand& buildInfo, const wxString& fileName = wxEmptyString, bool runPremakeOnly = false,
                   bool preprocessOnly = false);

    /// dtor
    virtual ~CompileRequest();

    // process the request
    virtual void Process(IManager* manager = NULL);

    // setters/getters
    const wxString& GetProjectName() const { return m_info.GetProject(); }
};

#endif // COMPILE_REQUEST_H
