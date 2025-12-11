#pragma once

#include "IPathConverter.hpp"
#include "LSP/LSPEvent.h"
#include "LSP/MessageWithParams.h"

#include <functional>
#include <wx/filename.h>
#include <wx/string.h>
#include <wx/thread.h>

namespace LSP
{

class WXDLLIMPEXP_CL Request : public LSP::MessageWithParams
{
public:
    Request();
    ~Request() override = default;

    void SetId(int id) { this->m_id = id; }
    int GetId() const { return m_id; }

    virtual JSONItem ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONItem& json);

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

    /**
     * @brief this method will get called by the protocol for handling the response.
     * Override it in the various requests
     */
    virtual void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
    {
        wxUnusedVar(response);
        wxUnusedVar(owner);
    }

    /**
     * @brief this method will get called by the protocol for handling the errors
     */
    virtual void OnError(const LSP::ResponseMessage& response, wxEvtHandler* owner)
    {
        wxUnusedVar(response);
        wxUnusedVar(owner);
    }

    void SetServerName(const wxString& server_name) { this->m_server_name = server_name; }
    const wxString& GetServerName() const { return m_server_name; }

    inline void SetResponseCallback(std::function<void(const LSPEvent&)> cb) { m_on_response_callback = std::move(cb); }
    inline void InvokeResponseCallback(const LSPEvent& event)
    {
        if (!wxThread::IsMain() || !m_on_response_callback) {
            return;
        }
        m_on_response_callback(event);
    }

protected:
    int m_id = wxNOT_FOUND;
    wxString m_server_name;
    std::function<void(const LSPEvent&)> m_on_response_callback{nullptr};
};
} // namespace LSP
