#ifndef DOCUMDENET_SYMBOLS_REQUEST_HPP
#define DOCUMDENET_SYMBOLS_REQUEST_HPP

#include "LSP/Request.h"
#include "LSP/basic_types.h"

#include <vector>

namespace LSP
{
class WXDLLIMPEXP_CL DocumentSymbolsRequest : public LSP::Request
{
public:
    enum eDocumentSymbolsContext {
        CONTEXT_NONE = 0,
        CONTEXT_SEMANTIC_HIGHLIGHT = 1 << 0,
        CONTEXT_OUTLINE_VIEW = 1 << 2,
    };

    explicit DocumentSymbolsRequest(const wxString& filename, size_t context);
    virtual ~DocumentSymbolsRequest() = default;
    std::optional<LSPEvent> OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;

private:
    size_t m_context = CONTEXT_NONE;
    void QueueEvent(wxEvtHandler* owner,
                    const std::vector<LSP::SymbolInformation>& symbols,
                    const wxString& filename,
                    const wxEventType& event_type);
    LSPEvent CreateLSPEvent(const std::vector<LSP::SymbolInformation>& symbols,
                            const wxString& filename,
                            const wxEventType& event_type);
    std::vector<LSP::DocumentSymbol> FromDocumentSymbolsArray(JSONItem&& result);
    std::vector<LSP::SymbolInformation> FromSymbolInformationArray(JSONItem&& result);
};
} // namespace LSP
#endif // DOCUMDENET_SYMBOLS_REQUEST_HPP