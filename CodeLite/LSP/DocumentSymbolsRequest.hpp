#ifndef DOCUMDENET_SYMBOLS_REQUEST_HPP
#define DOCUMDENET_SYMBOLS_REQUEST_HPP

#include "LSP/Request.h"
#include "basic_types.h"
#include <wx/filename.h>

namespace LSP
{
class WXDLLIMPEXP_CL DocumentSymbolsRequest : public LSP::Request
{
    // Some LSPs do not provide semanticTokensProvider support
    // we work around it by using this message to get list of
    // symbols in the current document
    bool m_forSemanticHighlight = false;

public:
    explicit DocumentSymbolsRequest(const wxString& filename, bool forSemanticHighlight);
    virtual ~DocumentSymbolsRequest();
    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner);
};
};     // namespace LSP
#endif // DOCUMDENET_SYMBOLS_REQUEST_HPP