#ifndef DOCUMDENET_SYMBOLS_REQUEST_HPP
#define DOCUMDENET_SYMBOLS_REQUEST_HPP

#include "LSP/Request.h"
#include "basic_types.h"
#include <wx/filename.h>

namespace LSP
{
class WXDLLIMPEXP_CL DocumentSymbolsRequest : public LSP::Request
{
public:
    DocumentSymbolsRequest(const wxFileName& filename);
    virtual ~DocumentSymbolsRequest();
    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner, IPathConverter::Ptr_t pathConverter);
};
};     // namespace LSP
#endif // DOCUMDENET_SYMBOLS_REQUEST_HPP