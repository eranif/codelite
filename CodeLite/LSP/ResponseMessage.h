#ifndef RESPONSEMESSAGE_H
#define RESPONSEMESSAGE_H

#include "JSON.h"
#include "LSP/Message.h"
#include "LSP/basic_types.h"

#include <macros.h>
#include <wx/sharedptr.h>

namespace LSP
{

class WXDLLIMPEXP_CL ResponseMessage : public LSP::Message
{
    int m_id = wxNOT_FOUND;
    std::unique_ptr<JSON> m_json;

public:
    ResponseMessage(std::unique_ptr<JSON>&& json);
    ~ResponseMessage() override;
    JSONItem ToJSON(const wxString& name) const override;
    void FromJSON(const JSONItem& json) override;

    std::string ToString() const override;
    ResponseMessage& SetId(int id)
    {
        this->m_id = id;
        return *this;
    }
    int GetId() const { return m_id; }
    bool IsOk() const { return m_json && m_json->isOk(); }
    std::unique_ptr<JSON> take() { return std::move(m_json); }

    bool IsErrorResponse() const;
    bool Has(const wxString& property) const;
    JSONItem Get(const wxString& property) const;

    JSONItem operator[](const wxString& name) const { return Get(name); }

    /**
     * @brief is this a "textDocument/publishDiagnostics" message?
     */
    bool IsPushDiagnostics() const { return Get("method").toString() == "textDocument/publishDiagnostics"; }

    /**
     * @brief return list of diagnostics
     */
    std::vector<LSP::Diagnostic> GetDiagnostics() const;
    /**
     * @brief return the URI diagnostics
     */
    wxString GetDiagnosticsUri() const;
};
}; // namespace LSP

#endif // RESPONSEMESSAGE_H
