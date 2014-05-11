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
