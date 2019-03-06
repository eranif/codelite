#ifndef RESPONSEMESSAGE_H
#define RESPONSEMESSAGE_H

#include "json_rpc/Message.h"
#include <wx/sharedptr.h>

namespace json_rpc
{

class WXDLLIMPEXP_CL ResponseMessage : public json_rpc::Message
{
    int m_id = wxNOT_FOUND;
    wxSharedPtr<JSON> m_json;

public:
    ResponseMessage(const wxString& message);
    virtual ~ResponseMessage();
    virtual JSONItem ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONItem& json);

    virtual wxString ToString() const;
    ResponseMessage& SetId(int id)
    {
        this->m_id = id;
        return *this;
    }
    int GetId() const { return m_id; }
    
    bool IsOk() const { return m_json != nullptr; }
    bool Has(const wxString& property) const;
    JSONItem Get(const wxString& property) const;
};

}; // namespace json_rpc

#endif // RESPONSEMESSAGE_H
