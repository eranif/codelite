#ifndef CLJSONRPC_H
#define CLJSONRPC_H

#include "codelite_exports.h"
#include "JSON.h"
#include "basic_types.h"
#include <wx/event.h>

namespace json_rpc
{

class WXDLLIMPEXP_CL Sender : public wxEvtHandler
{
public:
    /**
     * @brief send a JSONRPC request object (represented as a string) over the network
     * The response is received as an event of type ResponseEvent
     */
    virtual void Send(const wxString& message) = 0;
};

};     // namespace json_rpc
#endif // CLJSONRPC_H
