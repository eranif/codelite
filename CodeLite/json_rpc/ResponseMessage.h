#ifndef RESPONSEMESSAGE_H
#define RESPONSEMESSAGE_H

#include "json_rpc/Message.h"

namespace json_rpc
{

class WXDLLIMPEXP_CL ResponseMessage : public json_rpc::Message
{
    int m_id = wxNOT_FOUND;

public:
    ResponseMessage();
    virtual ~ResponseMessage();
    virtual wxString ToString() const;

    virtual JSONItem ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONItem& json);

    ResponseMessage& SetId(int id)
    {
        this->m_id = id;
        return *this;
    }
    int GetId() const { return m_id; }
};

}; // namespace json_rpc

#endif // RESPONSEMESSAGE_H
