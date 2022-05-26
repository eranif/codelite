#include "GotoDefinitionRequest.h"

#include "LSP/LSPEvent.h"
#include "file_logger.h"

LSP::GotoDefinitionRequest::GotoDefinitionRequest(const wxString& filename, size_t line, size_t column)
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

void LSP::GotoDefinitionRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    JSONItem result = response.Get("result");
    if(!result.isOk()) {
        return;
    }

    std::vector<LSP::Location> locations;
    if(result.isArray()) {
        int count = result.arraySize();
        locations.reserve(count);
        for(int i = 0; i < count; ++i) {
            LSP::Location loc;
            loc.FromJSON(result.arrayItem(i));
            locations.emplace_back(loc);
        }
    } else {
        LSP::Location loc;
        loc.FromJSON(result);
        locations.push_back(loc);
    }

    if(locations.empty()) {
        return;
    }

    // Fire an event with the matching location
    LSPEvent definitionEvent(wxEVT_LSP_DEFINITION);
    definitionEvent.SetLocations(locations);
    owner->AddPendingEvent(definitionEvent);
}

bool LSP::GotoDefinitionRequest::IsValidAt(const wxString& filename, size_t line, size_t col) const
{
    return (m_filename == filename) && (m_line == line) && (m_column == col);
}
