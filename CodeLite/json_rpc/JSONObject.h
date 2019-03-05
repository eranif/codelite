#ifndef JSONObject_H
#define JSONObject_H

#include "codelite_exports.h"
#include "JSON.h"

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
    virtual JSONItem ToJSON(const wxString& name) const = 0;
    virtual void FromJSON(const JSONItem& json) = 0;
};

};     // namespace json_rpc
#endif // JSONObject_H
