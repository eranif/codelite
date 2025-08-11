#ifndef CHILDPROCESS_H
#define CHILDPROCESS_H

#if defined(__WXMSW__)
#define USE_IPROCESS 1
#else
#define USE_IPROCESS 0
#include "UnixProcess.h"
#endif

#include "asyncprocess.h"
#include "codelite_exports.h"

#include <memory>
#include <wx/arrstr.h>
#include <wx/event.h>

class WXDLLIMPEXP_CL ChildProcess : public wxEvtHandler
{
#if USE_IPROCESS
    std::unique_ptr<IProcess> m_process;
#else
    std::unique_ptr<UnixProcess> m_childProcess;
#endif
public:
    ChildProcess() = default;
    ~ChildProcess() override;

    void Start(const wxArrayString& args);
    void Write(const wxString& message);
    void Write(const std::string& message);
    bool IsOk() const;
};

#endif // CHILDPROCESS_H
