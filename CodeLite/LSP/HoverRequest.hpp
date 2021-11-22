#ifndef HOVERREQUEST_HPP
#define HOVERREQUEST_HPP

#include "LSP/Request.h"
#include "LSP/ResponseMessage.h"
#include <wx/filename.h>

namespace LSP
{
class WXDLLIMPEXP_CL HoverRequest : public LSP::Request
{
    wxString m_filename;
    size_t m_line = 0;
    size_t m_column = 0;

public:
    explicit HoverRequest(const wxString& filename, size_t line, size_t column);
    virtual ~HoverRequest();

    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner);
};
};     // namespace LSP
#endif // HOVERREQUEST_HPP
