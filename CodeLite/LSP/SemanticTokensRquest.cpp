#include "SemanticTokensRquest.hpp"

#include "LSP/LSPEvent.h"
#include "LSP/basic_types.h"
#include "file_logger.h"
#include "json_rpc_params.h"
#include "wx/vector.h"

#include <thread>
#include <vector>

LSP::SemanticTokensRquest::SemanticTokensRquest(const wxString& filename)
    : m_filename(filename)
{
    SetMethod("textDocument/semanticTokens/full");
    m_params.reset(new SemanticTokensParams());
    m_params->As<SemanticTokensParams>()->SetTextDocument(filename);
}

LSP::SemanticTokensRquest::~SemanticTokensRquest() {}

void LSP::SemanticTokensRquest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    // build set of classes, locals so we can colour them
    if(!owner) {
        return;
    }

    std::vector<int> encoded_types;
    LOG_IF_TRACE { clTRACE() << "OnResponse for SemanticTokensRquest is called" << endl; }

    encoded_types = response["result"]["data"].toIntArray();

    LOG_IF_TRACE { clTRACE() << "Parsing semantic tokens array (" << encoded_types.size() << ")" << endl; }

    // since this is CPU heavy processing, spawn a thread to do the job
    wxString filename = m_filename;
    wxString server_name = GetServerName();

    // sanity: each token is represented by a set of 5 integers
    // { line, startChar, length, tokenType, tokenModifiers}
    if(encoded_types.size() % 5 != 0) {
        return;
    }

    int last_line = 0;
    int last_column = 0;
    std::vector<LSP::SemanticTokenRange> semantic_tokens;
    semantic_tokens.reserve(encoded_types.size() / 5);

    for(size_t i = 0; i < encoded_types.size() / 5; i++) {
        size_t base_index = 5 * i;
        LSP::SemanticTokenRange t;
        // calculate the token line
        t.line = last_line + encoded_types[base_index];

        // did we change lines?
        bool changed_line = t.line != last_line;

        // incase we are on a different line, the start_col is relative to 0, otherwise
        // it is relative to the previous item column
        t.column = changed_line ? encoded_types[base_index + 1] : encoded_types[base_index + 1] + last_column;
        t.length = encoded_types[base_index + 2];
        t.token_type = encoded_types[base_index + 3];

        last_column = t.column;
        last_line = t.line;
        semantic_tokens.emplace_back(t);
    }

    LSPEvent event(wxEVT_LSP_SEMANTICS);
    event.SetSemanticTokens(semantic_tokens);
    event.SetFileName(filename);
    event.SetServerName(GetServerName());
    owner->AddPendingEvent(event);
    LOG_IF_DEBUG { clDEBUG() << "Colouring file:" << filename << endl; }
}
