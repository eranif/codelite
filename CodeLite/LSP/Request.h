#ifndef LSP_REQUEST_H
#define LSP_REQUEST_H

#include "LSP/MessageWithParams.h"

namespace LSP
{

class WXDLLIMPEXP_SDK Request : public LSP::MessageWithParams
{
    int m_id = wxNOT_FOUND;

public:
    Request();
    virtual ~Request();

    void SetId(int id) { this->m_id = id; }
    int GetId() const { return m_id; }

    virtual JSONItem ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONItem& json);

    /**
     * @brief this method will get called by the protocol for handling the response.
     * Override it in the various requests
     */
    virtual void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) = 0;
};
}; // namespace LSP

#endif // REQUEST_H
