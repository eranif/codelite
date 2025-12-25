#ifndef JSONObject_H
#define JSONObject_H

#include "JSON.h"
#include "codelite_exports.h"
#include "IPathConverter.hpp"

namespace LSP
{
//===-----------------------------------------------------------------------------------
// Base type class
//===-----------------------------------------------------------------------------------
class WXDLLIMPEXP_CL Serializable
{
public:
    Serializable() = default;
    virtual ~Serializable() = default;
    virtual JSONItem ToJSON(const wxString& name) const = 0;
    virtual void FromJSON(const JSONItem& json) = 0;

    bool operator==(const Serializable&) const = default;
    auto operator<=>(const Serializable&) const = default;
};

} // namespace LSP
#endif // JSONObject_H
