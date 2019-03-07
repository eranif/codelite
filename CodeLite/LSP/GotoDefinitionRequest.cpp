#include "GotoDefinitionRequest.h"

LSP::GotoDefinitionRequest::GotoDefinitionRequest(const wxFileName& filename, size_t line, size_t column)
    : m_filename(filename)
    , m_line(line)
    , m_column(column)
{
    SetMethod("textDocument/definition");
    m_params.reset(new TextDocumentPositionParams());
    m_params->As<TextDocumentPositionParams>()->SetTextDocument(TextDocumentIdentifier(filename));
    m_params->As<TextDocumentPositionParams>()->SetPosition(Position(line, column));
}

LSP::GotoDefinitionRequest::~GotoDefinitionRequest() {}

void LSP::GotoDefinitionRequest::OnReponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    // {"id":2,"jsonrpc":"2.0","result":[{"range":{"end":{"character":38,"line":73},"start":{"character":32,"line":73}},"uri":"file:///usr/include/c%2b%2b/6/bits/stringfwd.h"},{"range":{"end":{"character":38,"line":73},"start":{"character":32,"line":73}},"uri":"file:///usr/lib/gcc/x86_64-linux-gnu/6.3.0/../../../../include/c%2b%2b/6.3.0/bits/stringfwd.h"}]}
    JSONItem result = response.Get("result");
    if(!result.isOk()) { return; }
    LSP::Location loc;
    if(result.isArray()) {
        loc.FromJSON(result.arrayItem(0));
    } else {
        loc.FromJSON(result);
    }
}
