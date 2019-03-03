#ifndef JSONObject_H
#define JSONObject_H

#include "codelite_exports.h"
#include "json_node.h"

namespace json_rpc
{
//===-----------------------------------------------------------------------------------
// Base type class
//===-----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL Serializable
{
public:
    Serializable() {}
    virtual ~Serializable() {}
    virtual JSONElement ToJSON(const wxString& name) const = 0;
    virtual void FromJSON(const JSONElement& json) = 0;
};

};     // namespace json_rpc
#endif // JSONObject_H
