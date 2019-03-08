#ifndef CLJSONRPC_H
#define CLJSONRPC_H

#include "codelite_exports.h"
#include "JSON.h"
#include "basic_types.h"
#include <wx/event.h>

namespace LSP
{

class WXDLLIMPEXP_CL Sender : public wxEvtHandler
{
public:
    /**
     * @brief send a JSONRPC request object (represented as a string) over the network
     * The response is received as an event of type ResponseEvent
     */
    virtual void Send(const std::string& message) = 0;
};

};     // namespace LSP
#endif // CLJSONRPC_H
