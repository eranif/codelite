#ifndef CSCOMMANDHANDLERBASE_H
#define CSCOMMANDHANDLERBASE_H

#include "file_logger.h"
#include "json_node.h"
#include <cl_command_event.h>
#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wx/string.h>

wxDECLARE_EVENT(wxEVT_COMMAND_PROCESSED, clCommandEvent);

#define CHECK_OPTION(str_option)                                \
    if(!options.hasNamedObject(str_option)) {                   \
        clERROR() << "Command is missing field:" << str_option; \
        NotifyCompletion();                                     \
        return;                                                 \
    }

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
    
    wxEvtHandler* GetSink() { return m_sink; }
    
    /**
     * @brief process a request from the command line and print the result to the stdout
     * @param the handler options
     */
    virtual void Process(const JSONElement& options) = 0;
};

#endif // CSCOMMANDHANDLERBASE_H
