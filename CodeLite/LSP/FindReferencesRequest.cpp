#include "FindReferencesRequest.hpp"

#include "JSON.h"
#include "LSP/LSPEvent.h"
#include "event_notifier.h"
#include "file_logger.h"

LSP::FindReferencesRequest::FindReferencesRequest(const wxString& filename, size_t line, size_t column,
                                                  bool includeDeclaration)
{
    SetMethod("textDocument/references");
    m_params.reset(new ReferenceParams(includeDeclaration));
    m_params->As<ReferenceParams>()->SetTextDocument(TextDocumentIdentifier(filename));
    m_params->As<ReferenceParams>()->SetPosition(Position(line, column));
}

LSP::FindReferencesRequest::~FindReferencesRequest() {}

void LSP::FindReferencesRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    wxUnusedVar(owner);
    JSONItem result = response.Get("result");
    CHECK_EXPECTED_RETURN(result.isOk(), true);

    // An array of locations
    int array_size = result.arraySize();

    LSPEvent references_event{ wxEVT_LSP_REFERENCES };
    std::vector<LSP::Location>& locations = references_event.GetLocations();
    locations.reserve(array_size);

    for(int i = 0; i < array_size; ++i) {
        auto d = result[i];
        LSP::Location loc;
        loc.FromJSON(d);
        locations.emplace_back(loc);
    }

    // fire an event
    EventNotifier::Get()->AddPendingEvent(references_event);
}
