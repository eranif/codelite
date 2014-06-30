//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : file_logger.h
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

#ifndef FILELOGGER_H
#define FILELOGGER_H

#include <wx/ffile.h>
#include "codelite_exports.h"
#include <wx/stopwatch.h>

class WXDLLIMPEXP_CL FileLogger
{
public:
    enum {
        System   = -1,
        Error    =  0,
        Warning  =  1,
        Dbg      =  2,
        Developer=  3
    };

protected:
    int   m_verbosity;
    FILE* m_fp;

public:
    FileLogger();
    ~FileLogger();
    
    /**
     * @brief open the log file
     */
    static void OpenLog(const wxString& fullName, int verbosity);
    
    static FileLogger *Get();

    void AddLogLine(const wxString &msg, int verbosity);
    void SetVerbosity(int level);

    // Set the verbosity as string
    void SetVerbosity(const wxString &verbosity);

    // Various util methods
    static wxString GetVerbosityAsString(int verbosity);
    static int GetVerbosityAsNumber(const wxString &verbosity);

};

#define CL_SYSTEM(...)  FileLogger::Get()->AddLogLine(wxString::Format(__VA_ARGS__), FileLogger::System);
#define CL_ERROR(...)   FileLogger::Get()->AddLogLine(wxString::Format(__VA_ARGS__), FileLogger::Error);
#define CL_WARNING(...) FileLogger::Get()->AddLogLine(wxString::Format(__VA_ARGS__), FileLogger::Warning);
#define CL_DEBUG(...)   FileLogger::Get()->AddLogLine(wxString::Format(__VA_ARGS__), FileLogger::Dbg);
#define CL_DEBUGS(s)    FileLogger::Get()->AddLogLine(s, FileLogger::Dbg);
#define CL_DEBUG1(...)  FileLogger::Get()->AddLogLine(wxString::Format(__VA_ARGS__), FileLogger::Developer);


#endif // FILELOGGER_H
