#ifndef DOCUMDENET_SYMBOLS_REQUEST_HPP
#define DOCUMDENET_SYMBOLS_REQUEST_HPP

#include "LSP/Request.h"
#include "LSP/basic_types.h"

#include <vector>
#include <wx/filename.h>

namespace LSP
{
class WXDLLIMPEXP_CL DocumentSymbolsRequest : public LSP::Request
{
public:
    enum eDocumentSymbolsContext {
        CONTEXT_NONE = 0,
        CONTEXT_SEMANTIC_HIGHLIGHT = 1 << 0,
        CONTEXT_QUICK_OUTLINE = 1 << 1,
        CONTEXT_OUTLINE_VIEW = 1 << 2,
    };

private:
    size_t m_context = CONTEXT_NONE;

private:
    void QueueEvent(wxEvtHandler* owner, const std::vector<LSP::SymbolInformation>& symbols, const wxString& filename,
                    const wxEventType& event_type);

public:
    explicit DocumentSymbolsRequest(const wxString& filename, size_t context);
    virtual ~DocumentSymbolsRequest();
    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner);
};
};     // namespace LSP
#endif // DOCUMDENET_SYMBOLS_REQUEST_HPP