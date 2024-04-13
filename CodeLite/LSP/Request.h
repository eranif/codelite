#ifndef LSP_REQUEST_H
#define LSP_REQUEST_H

#include "IPathConverter.hpp"
#include "LSP/MessageWithParams.h"
#include "wx/string.h"

#include <wx/filename.h>

namespace LSP
{

class WXDLLIMPEXP_CL Request : public LSP::MessageWithParams
{
    int m_id = wxNOT_FOUND;
    wxString m_server_name;

public:
    Request();
    virtual ~Request();

    void SetId(int id) { this->m_id = id; }
    int GetId() const { return m_id; }

    virtual JSONItem ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONItem& json);

    /**
     * @brief is this request position dependant? (i.e. the response should be diplsayed where the request was
     * triggered?)
     */
    virtual bool IsPositionDependantRequest() const { return false; }

    /**
     * @brief in case 'IsPositionDependantRequest' is true, return true if the response is valid at
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
};
}; // namespace LSP

#endif // REQUEST_H
