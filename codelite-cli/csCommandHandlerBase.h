#ifndef CSCOMMANDHANDLERBASE_H
#define CSCOMMANDHANDLERBASE_H

//#include <clNetworkMessage.h>
//#include <SocketAPI/clSocketBase.h>
#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wx/string.h>
#include <cl_command_event.h>

wxDECLARE_EVENT(wxEVT_COMMAND_PROCESSED, clCommandEvent);

class csCommandHandlerBase
{
    wxEvtHandler* m_sink;

public:
    typedef wxSharedPtr<csCommandHandlerBase> Ptr_t;

protected:
    void NotifyCompletion();

public:
    csCommandHandlerBase(wxEvtHandler* sink);
    virtual ~csCommandHandlerBase();

    /**
     * @brief process a request from the command line and print the result to the stdout
     * @param message
     */
    virtual void Process(const wxString& message) = 0;
};

#endif // CSCOMMANDHANDLERBASE_H
