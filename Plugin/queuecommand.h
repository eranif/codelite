//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : queuecommand.h
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

#ifndef __queuecommand__
#define __queuecommand__

#include "codelite_exports.h"

#include <wx/string.h>

class WXDLLIMPEXP_SDK QueueCommand
{
    wxString m_project;
    wxString m_configuration;
    bool m_projectOnly;
    int m_kind;
    bool m_cleanLog;
    bool m_checkBuildSuccess;
    wxString m_customBuildTarget;

public:
    // Command's kind
    enum { kBuild, kClean, kCustomBuild, kDebug, kRebuild, kExecuteNoDebug };

public:
    QueueCommand(const wxString& project, const wxString& configuration, bool projectOnly, int kind);
    QueueCommand(int kind);
    ~QueueCommand();

    //----------------------------------------
    // Setters/Getters
    //----------------------------------------

    void SetConfiguration(const wxString& configuration);
    void SetProject(const wxString& project);
    const wxString& GetConfiguration() const;
    const wxString& GetProject() const;
    void SetProjectOnly(const bool& projectOnly);
    const bool& GetProjectOnly() const;
    void SetKind(const int& kind);
    const int& GetKind() const;
    void SetCleanLog(const bool& cleanLog);
    const bool& GetCleanLog() const;
    void SetCheckBuildSuccess(const bool& checkBuildSuccess) { this->m_checkBuildSuccess = checkBuildSuccess; }
    const bool& GetCheckBuildSuccess() const { return m_checkBuildSuccess; }
    void SetCustomBuildTarget(const wxString& customBuildTarget) { this->m_customBuildTarget = customBuildTarget; }
    const wxString& GetCustomBuildTarget() const { return m_customBuildTarget; }
};

#endif // __queuecommand__
