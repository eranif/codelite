#ifndef MESSAGE_H
#define MESSAGE_H

#include "IPathConverter.hpp"
#include "JSON.h"
#include "JSONObject.h"
#include "codelite_exports.h"
#include <string>
#include <memory>

namespace LSP
{
class WXDLLIMPEXP_CL Message : public LSP::Serializable
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
    virtual std::string ToString() const = 0;

    /**
     * @brief return the **first** JSON payload from the network buffer
     * @param network_buffer - network buffer (may contain multiple messages)
     */
    static std::unique_ptr<JSON> GetJSONPayload(wxString& network_buffer);

    template <typename T> T* As() const { return dynamic_cast<T*>(const_cast<Message*>(this)); }
};

}; // namespace LSP

#endif // MESSAGE_H
