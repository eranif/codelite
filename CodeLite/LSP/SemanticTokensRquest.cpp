#include "SemanticTokensRquest.hpp"
#include "LSP/basic_types.h"
#include "json_rpc_params.h"

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
    wxUnusedVar(response);
    wxUnusedVar(owner);
}
