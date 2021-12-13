#include "LSP/DocumentSymbolsRequest.hpp"
#include "LSP/LSPEvent.h"
#include "file_logger.h"
#include "json_rpc_params.h"
#include <algorithm>

namespace
{
FileLogger& operator<<(FileLogger& logger, const std::vector<LSP::SymbolInformation>& symbols)
{
    wxString s;
    s << "\n[\n";
    for(const LSP::SymbolInformation& d : symbols) {
        s << "  " << d.GetContainerName() << "." << d.GetName() << ",\n";
    }
    s << "]\n";
    logger.Append(s, logger.GetRequestedLogLevel());
    return logger;
}
} // namespace

LSP::DocumentSymbolsRequest::DocumentSymbolsRequest(const wxString& filename, size_t context)
    : m_context(context)
{
    SetMethod("textDocument/documentSymbol");
    // set the params
    m_params.reset(new DocumentSymbolParams());
    m_params->As<DocumentSymbolParams>()->SetTextDocument(TextDocumentIdentifier(filename));
}

LSP::DocumentSymbolsRequest::~DocumentSymbolsRequest() {}

void LSP::DocumentSymbolsRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    auto result = response.Get("result");
    if(!result.isOk()) {
        clWARNING() << "LSP::DocumentSymbolsRequest::OnResponse(): invalid 'result' object";
        return;
    }

    if(result.isArray()) {
        int size = result.arraySize();
        if(size == 0) {
            return;
        }

        clDEBUG1() << result.format() << endl;
        if(result[0].hasNamedObject("location")) {
            wxString filename = m_params->As<DocumentSymbolParams>()->GetTextDocument().GetPath();
            std::vector<LSP::SymbolInformation> symbols;
            symbols.reserve(size);
            for(int i = 0; i < size; ++i) {
                SymbolInformation si;
                si.FromJSON(result[i]);
                symbols.push_back(si);
            }

            // sort the items by line position
            std::sort(symbols.begin(), symbols.end(),
                      [=](const SymbolInformation& a, const SymbolInformation& b) -> int {
                          const LSP::Location& loc_a = a.GetLocation();
                          const LSP::Location& loc_b = b.GetLocation();
                          return loc_a.GetRange().GetStart().GetLine() < loc_b.GetRange().GetStart().GetLine();
                      });
            clDEBUG1() << symbols << endl;

            // fire event per context
            if(m_context & CONTEXT_SEMANTIC_HIGHLIGHT) {
                QueueEvent(owner, symbols, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_FOR_HIGHLIGHT);
            }
            if(m_context & CONTEXT_QUICK_OUTLINE) {
                QueueEvent(owner, symbols, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_QUICK_OUTLINE);
            }
            if(m_context & CONTEXT_OUTLINE_VIEW) {
                QueueEvent(owner, symbols, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW);
            }
        } else {
            std::vector<DocumentSymbol> symbols;
            symbols.reserve(size);
            for(int i = 0; i < size; ++i) {
                DocumentSymbol ds;
                ds.FromJSON(result[i]);
                symbols.push_back(ds);
            }
            wxUnusedVar(symbols);
        }
    }
}

void LSP::DocumentSymbolsRequest::QueueEvent(wxEvtHandler* owner, const std::vector<LSP::SymbolInformation>& symbols,
                                             const wxString& filename, const wxEventType& event_type)
{
    LSPEvent event{ event_type };
    event.GetSymbolsInformation().reserve(symbols.size());
    event.GetSymbolsInformation().insert(event.GetSymbolsInformation().end(), symbols.begin(), symbols.end());
    event.SetFileName(filename);
    owner->QueueEvent(event.Clone());
}
