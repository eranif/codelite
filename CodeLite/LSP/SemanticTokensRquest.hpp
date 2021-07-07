#ifndef SEMANTICTOKENSRQUEST_HPP
#define SEMANTICTOKENSRQUEST_HPP

#include "LSP/Request.h"
#include "basic_types.h"
#include "codelite_exports.h"

namespace LSP
{
class WXDLLIMPEXP_CL SemanticTokensRquest : public Request
{
    wxString m_filename;

public:
    SemanticTokensRquest(const wxString& filename);
    ~SemanticTokensRquest();

    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner);
};
} // namespace LSP

#endif // SEMANTICTOKENSRQUEST_HPP
