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

LSP::DocumentSymbolsRequest::DocumentSymbolsRequest(const wxString& filename, bool forSemanticHighlight)
    : m_forSemanticHighlight(forSemanticHighlight)
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
            // only SymbolInformation has the `location` property
            // fire an event with all the symbols
            LSPEvent event(m_forSemanticHighlight ? wxEVT_LSP_DOCUMENT_SYMBOLS_FOR_HIGHLIGHT
                                                  : wxEVT_LSP_DOCUMENT_SYMBOLS);
            event.GetSymbolsInformation().reserve(size);
            event.SetFileName(m_params->As<DocumentSymbolParams>()->GetTextDocument().GetPath());
            for(int i = 0; i < size; ++i) {
                SymbolInformation si;
                si.FromJSON(result[i]);
                event.GetSymbolsInformation().push_back(si);
            }

            // sort the items by line position
            auto& items = event.GetSymbolsInformation();
            std::sort(items.begin(), items.end(), [=](const SymbolInformation& a, const SymbolInformation& b) -> int {
                const LSP::Location& loc_a = a.GetLocation();
                const LSP::Location& loc_b = b.GetLocation();
                return loc_a.GetRange().GetStart().GetLine() < loc_b.GetRange().GetStart().GetLine();
            });
            clDEBUG1() << event.GetSymbolsInformation() << endl;
            owner->QueueEvent(event.Clone());
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
