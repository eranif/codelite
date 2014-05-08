#ifndef COMPILECOMMANDSCREATEOR_H
#define COMPILECOMMANDSCREATEOR_H

#include "job.h"
#include <wx/thread.h>
#include "codelite_exports.h"
#include <wx/filename.h>
#include "cl_command_event.h"

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_COMPILE_COMMANDS_JSON_GENERATED, clCommandEvent);

class WXDLLIMPEXP_SDK CompileCommandsCreateor : public Job
{
    wxFileName m_filename;
public:
    CompileCommandsCreateor(const wxFileName& path);
    virtual ~CompileCommandsCreateor();

public:
    virtual void Process(wxThread* thread);
};

#endif // COMPILECOMMANDSCREATEOR_H
