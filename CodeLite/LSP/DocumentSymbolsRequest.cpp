#include "LSP/DocumentSymbolsRequest.hpp"
#include "LSP/LSPEvent.h"
#include "file_logger.h"
#include "json_rpc_params.h"

LSP::DocumentSymbolsRequest::DocumentSymbolsRequest(const wxFileName& filename)
{
    SetMethod("textDocument/documentSymbol");
    // set the params
    m_params.reset(new DocumentSymbolParams());
    m_params->As<DocumentSymbolParams>()->SetTextDocument(TextDocumentIdentifier(filename));
}

LSP::DocumentSymbolsRequest::~DocumentSymbolsRequest() {}

void LSP::DocumentSymbolsRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner,
                                             IPathConverter::Ptr_t pathConverter)
{
    auto result = response.Get("result");
    if(!result.isOk()) {
        clWARNING() << "LSP::DocumentSymbolsRequest::OnResponse(): invalid 'result' object";
        return;
    }

    clDEBUG() << result.format() << clEndl;
    if(result.isArray()) {
        int size = result.arraySize();
        if(size == 0) {
            return;
        }

        if(result[0].hasNamedObject("containerName")) {
            // fire an event with all the symbols
            LSPEvent event(wxEVT_LSP_DOCUMENT_SYMBOLS);
            event.GetSymbolsInformation().reserve(size);
            for(int i = 0; i < size; ++i) {
                SymbolInformation si;
                si.FromJSON(result[i], pathConverter);
                event.GetSymbolsInformation().push_back(si);
            }
            owner->QueueEvent(event.Clone());
        } else {
            std::vector<DocumentSymbol> symbols;
            for(int i = 0; i < size; ++i) {
                DocumentSymbol ds;
                ds.FromJSON(result[i], pathConverter);
                symbols.push_back(ds);
            }
        }
    }
}
