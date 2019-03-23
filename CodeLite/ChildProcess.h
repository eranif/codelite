#ifndef CHILDPROCESS_H
#define CHILDPROCESS_H

#if defined(__WXMSW__)
#define USE_IPROCESS 1
#else
#define USE_IPROCESS 0
#include "UnixProcess.h"
#endif

#include <wx/arrstr.h>
#include <wx/event.h>
#include "cl_command_event.h"
#include <asyncprocess.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL ChildProcess : public wxEvtHandler
{
#if USE_IPROCESS
    IProcess* m_process = nullptr;
#else
    UnixProcess* m_childProcess = nullptr;
#endif
public:
    ChildProcess();
    virtual ~ChildProcess();

    void Start(const wxArrayString& args);
    void Write(const wxString& message);
    void Write(const std::string& message);
};

#endif // CHILDPROCESS_H
