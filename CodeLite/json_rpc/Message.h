#ifndef MESSAGE_H
#define MESSAGE_H

#include "codelite_exports.h"
#include "JSONObject.h"
#include "JSON.h"

namespace json_rpc
{
class WXDLLIMPEXP_CL Message : public json_rpc::Serializable
{
    wxString m_jsonrpc = "2.0";

public:
    static int GetNextID();

public:
    Message();
    virtual ~Message();
    virtual JSONItem ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONItem& json);

    /**
     * @brief serialize this message into string
     */
    virtual wxString ToString() const = 0;
};

}; // namespace json_rpc

#endif // MESSAGE_H
