#ifndef REQUESTMESSAGE_H
#define REQUESTMESSAGE_H

#include "LSP/Message.h" // Base class: LSP::Message
#include "LSP/basic_types.h"
#include "json_rpc_params.h"
#include <wx/sharedptr.h>
#include <wx/event.h>
#include "LSP/ResponseMessage.h"

namespace LSP
{

class WXDLLIMPEXP_CL RequestMessage : public LSP::Message
{
public:
    typedef wxSharedPtr<LSP::RequestMessage> Ptr_t;

protected:
    int m_id = wxNOT_FOUND;
    wxString m_method;
    Params::Ptr_t m_params;
    wxString m_uuid;
    wxString m_statusMessage; // A custom message to be displayed to the user when this message is being processed
    bool m_needsReply = false;

public:
    RequestMessage();
    virtual ~RequestMessage();
    virtual JSONItem ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONItem& json);
    virtual std::string ToString() const;

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
    virtual void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner);

    void SetNeedsReply(bool needsReply) { this->m_needsReply = needsReply; }
    bool IsNeedsReply() const { return m_needsReply; }
    
    /**
     * @brief factory method for creating Ptr_t type
     */
    static LSP::RequestMessage::Ptr_t MakeRequest(LSP::RequestMessage* message_ptr);

    /**
     * @brief return a unique ID identifying this request
     */
    const wxString& GetUID() const { return m_uuid; }

    /**
     * @brief build the request UID
     */
    virtual void BuildUID() = 0;

    void SetStatusMessage(const wxString& statusMessage) { this->m_statusMessage = statusMessage; }
    const wxString& GetStatusMessage() const { return m_statusMessage; }
};

} // namespace LSP

#endif // REQUESTMESSAGE_H
