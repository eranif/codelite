#pragma once

#include "LSP/LSPEvent.h"
#include "LSP/MessageWithParams.h"

#include <functional>
#include <optional>
#include <wx/string.h>
#include <wx/thread.h>

namespace LSP
{

using ResponseCallback = std::function<void(std::optional<LSPEvent>)>;

class WXDLLIMPEXP_CL Request : public LSP::MessageWithParams
{
public:
    Request();
    ~Request() override = default;

    void SetId(int id) { this->m_id = id; }
    int GetId() const { return m_id; }

    JSONItem ToJSON() const override;
    void FromJSON(const JSONItem& json) override;

    /**
     * @brief is this request position dependent? (i.e. the response should be displayed where the request was
     * triggered?)
     */
    virtual bool IsPositionDependentRequest() const { return false; }

    /**
     * @brief in case 'IsPositionDependentRequest' is true, return true if the response is valid at
     * a given position. Usually, when the user moves while waiting for a response, it makes no sense on
     * displaying the response in the wrong location...
     */
    virtual bool IsValidAt(const wxString& filename, size_t line, size_t col) const
    {
        wxUnusedVar(filename);
        wxUnusedVar(line);
        wxUnusedVar(col);
        return true;
    }

    virtual std::optional<LSPEvent> OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) = 0;

    /**
     * @brief this method will get called by the protocol for handling the response.
     * Override it in the various requests
     */
    void HandleResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
    {
        auto event = OnResponse(response, owner);
        if (event.has_value()) {
            InvokeResponseCallback(event.value());
        }
    }

    /**
     * @brief this method will get called by the protocol for handling the errors
     */
    virtual void HandleError(const LSP::ResponseMessage& response, wxEvtHandler* owner)
    {
        wxUnusedVar(response);
        wxUnusedVar(owner);
    }

    void SetServerName(const wxString& server_name) { this->m_server_name = server_name; }
    const wxString& GetServerName() const { return m_server_name; }

    inline void SetResponseCallback(ResponseCallback cb) { m_on_response_callback = std::move(cb); }
    inline void InvokeResponseCallback(std::optional<LSPEvent> event)
    {
        if (!wxThread::IsMain() || !m_on_response_callback) {
            return;
        }
        m_on_response_callback(event);
    }

protected:
    int m_id = wxNOT_FOUND;
    wxString m_server_name;
    ResponseCallback m_on_response_callback{nullptr};
};
} // namespace LSP
