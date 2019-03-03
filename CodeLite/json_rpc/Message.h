#ifndef MESSAGE_H
#define MESSAGE_H

#include "codelite_exports.h"
#include "JSONObject.h"
#include "json_node.h"

namespace json_rpc
{
class WXDLLIMPEXP_CL Message : public Serializable
{
    wxString m_jsonrpc = "2.0";

public:
    static int GetNextID();

public:
    Message();
    virtual ~Message();
    virtual JSONElement ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONElement& json);

    /**
     * @brief serialize this message into string
     */
    virtual wxString ToString() const = 0;
};

}; // namespace json_rpc

#endif // MESSAGE_H
