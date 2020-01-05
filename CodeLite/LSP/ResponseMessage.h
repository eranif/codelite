#ifndef RESPONSEMESSAGE_H
#define RESPONSEMESSAGE_H

#include "LSP/Message.h"
#include "LSP/basic_types.h"
#include <macros.h>
#include <wx/sharedptr.h>

namespace LSP
{

class WXDLLIMPEXP_CL ResponseMessage : public LSP::Message
{
    int m_id = wxNOT_FOUND;
    wxSharedPtr<JSON> m_json;
    wxString m_jsonMessage;
    IPathConverter::Ptr_t m_pathConverter;

protected:
    /**
     * @brief read headers from buffer. Return the number of bytes consumed
     */
    int ReadHeaders(const wxString& message, wxStringMap_t& headers);

public:
    ResponseMessage(wxString& message, IPathConverter::Ptr_t pathConverter);
    virtual ~ResponseMessage();
    virtual JSONItem ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const;
    virtual void FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter);

    virtual std::string ToString(IPathConverter::Ptr_t pathConverter) const;
    ResponseMessage& SetId(int id)
    {
        this->m_id = id;
        return *this;
    }
    const wxString& GetMessageString() const { return m_jsonMessage; }
    int GetId() const { return m_id; }
    bool IsOk() const { return m_json && m_json->isOk(); }
    bool Has(const wxString& property) const;
    JSONItem Get(const wxString& property) const;

    /**
     * @brief is this a "textDocument/publishDiagnostics" message?
     */
    bool IsPushDiagnostics() const { return Get("method").toString() == "textDocument/publishDiagnostics"; }

    /**
     * @brief return list of diagnostics
     */
    std::vector<LSP::Diagnostic> GetDiagnostics(IPathConverter::Ptr_t pathConverter) const;
    /**
     * @brief return the URI diagnostics
     */
    wxString GetDiagnosticsUri(IPathConverter::Ptr_t pathConverter) const;
};
}; // namespace LSP

#endif // RESPONSEMESSAGE_H
