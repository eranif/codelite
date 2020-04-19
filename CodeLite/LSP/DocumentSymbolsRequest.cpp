#include "DocumentSymbolsRequest.hpp"
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

    if(result.isArray()) {
        int size = result.arraySize();
        m_symbols.resize(size);
        for(int i = 0; i < size; ++i) {
            DocumentSymbol ds;
            ds.FromJSON(result[i], pathConverter);
            m_symbols.push_back(ds);
        }
    }
}
