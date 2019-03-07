#ifndef REQUESTMESSAGE_H
#define REQUESTMESSAGE_H

#include "json_rpc/Message.h" // Base class: json_rpc::Message
#include "clJSONRPC.h"
#include "json_rpc/basic_types.h"
#include "json_rpc_params.h"
#include <wx/sharedptr.h>
#include <wx/event.h>
#include "json_rpc/ResponseMessage.h"

namespace json_rpc
{

class WXDLLIMPEXP_CL RequestMessage : public json_rpc::Message
{
public:
    typedef wxSharedPtr<json_rpc::RequestMessage> Ptr_t;

protected:
    int m_id = wxNOT_FOUND;
    wxString m_method;
    Params::Ptr_t m_params;

public:
    RequestMessage();
    virtual ~RequestMessage();
    virtual JSONItem ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONItem& json);
    virtual wxString ToString() const;
    void Send(Sender* sender) const;

    void SetId(int id) { this->m_id = id; }
    void SetMethod(const wxString& method) { this->m_method = method; }
    int GetId() const { return m_id; }
    const wxString& GetMethod() const { return m_method; }
    void SetParams(Params::Ptr_t params) { m_params = params; }
    Params::Ptr_t GetParams() { return m_params; }
    
    /**
     * @brief this method will get called by the protocol for handling the response.
     * Override it in the various requests
     */
    virtual void OnReponse(const json_rpc::ResponseMessage& response, wxEvtHandler* owner)
    {
        wxUnusedVar(response);
        wxUnusedVar(owner);
    }
};

} // namespace json_rpc

#endif // REQUESTMESSAGE_H
