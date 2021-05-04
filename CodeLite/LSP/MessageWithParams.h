#ifndef REQUESTMESSAGE_H
#define REQUESTMESSAGE_H

#include "LSP/Message.h" // Base class: LSP::Message
#include "LSP/ResponseMessage.h"
#include "LSP/basic_types.h"
#include "json_rpc_params.h"
#include <wx/event.h>
#include <wx/sharedptr.h>

namespace LSP
{

class WXDLLIMPEXP_CL MessageWithParams : public LSP::Message
{
public:
    typedef wxSharedPtr<LSP::MessageWithParams> Ptr_t;

protected:
    wxString m_method;
    Params::Ptr_t m_params;
    wxString m_uuid;
    wxString m_statusMessage; // A custom message to be displayed to the user when this message is being processed

public:
    MessageWithParams();
    virtual ~MessageWithParams();
    virtual JSONItem ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONItem& json);
    virtual std::string ToString() const;

    void SetMethod(const wxString& method) { this->m_method = method; }
    const wxString& GetMethod() const { return m_method; }
    void SetParams(Params::Ptr_t params) { m_params = params; }
    Params::Ptr_t GetParams() { return m_params; }

    /**
     * @brief factory method for creating Ptr_t type
     */
    static LSP::MessageWithParams::Ptr_t MakeRequest(LSP::MessageWithParams* message_ptr);

    void SetStatusMessage(const wxString& statusMessage) { this->m_statusMessage = statusMessage; }
    const wxString& GetStatusMessage() const { return m_statusMessage; }
};

} // namespace LSP

#endif // REQUESTMESSAGE_H
