//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : clean_request.h
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
#ifndef CLEAN_REQUEST_H
#define CLEAN_REQUEST_H
#include "codelite_exports.h"
#include "shell_command.h"

/**
 * \class CleanRequest
 * \brief
 * \author Eran
 * \date 07/22/07
 */
class WXDLLIMPEXP_SDK CleanRequest : public ShellCommand
{
public:
    /**
     * Construct a compilation clean request. The compiler thread will clean the selected project and all
     * its dependencies as appeard in the build order dialog
     * \param projectName the selected project to build
     */
    CleanRequest(const QueueCommand& info);

    /// dtor
    virtual ~CleanRequest();

    // process the request
    virtual void Process(IManager* manager = NULL);

    // setters/getters
    const wxString& GetProjectName() const { return m_info.GetProject(); }
};

#endif // CLEAN_REQUEST_H
