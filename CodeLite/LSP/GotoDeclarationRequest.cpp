#include "GotoDeclarationRequest.h"
#include "LSP/LSPEvent.h"
#include "event_notifier.h"
#include "file_logger.h"

LSP::GotoDeclarationRequest::GotoDeclarationRequest(const wxString& filename, size_t line, size_t column,
                                                    bool for_add_missing_header)
    : m_filename(filename)
    , m_line(line)
    , m_column(column)
    , m_for_add_missing_header(for_add_missing_header)
{
    SetMethod("textDocument/declaration");
    m_params.reset(new TextDocumentPositionParams());
    m_params->As<TextDocumentPositionParams>()->SetTextDocument(TextDocumentIdentifier(filename));
    m_params->As<TextDocumentPositionParams>()->SetPosition(Position(line, column));
}

LSP::GotoDeclarationRequest::~GotoDeclarationRequest() {}

void LSP::GotoDeclarationRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    clDEBUG1() << "GotoDeclarationRequest::OnResponse() is called" << endl;
    JSONItem result = response.Get("result");
    if(!result.isOk()) {
        return;
    }

    LSP::Location loc;
    if(result.isArray()) {
        loc.FromJSON(result.arrayItem(0));
    } else {
        loc.FromJSON(result);
    }

    clDEBUG1() << result.format() << endl;

    if(!loc.GetPath().IsEmpty()) {
        if(m_for_add_missing_header) {
            LSPEvent event{ wxEVT_LSP_SYMBOL_DECLARATION_FOUND };
            event.SetLocation(loc);
            event.SetFileName(m_filename);
            EventNotifier::Get()->AddPendingEvent(event);
        } else {
            // We send the same event for declaraion as we do for definition
            LSPEvent event{ wxEVT_LSP_DEFINITION };
            event.SetLocation(loc);
            event.SetFileName(m_filename);
            owner->AddPendingEvent(event);
        }
    }
}

bool LSP::GotoDeclarationRequest::IsValidAt(const wxString& filename, size_t line, size_t col) const
{
    return (m_filename == filename) && (m_line == line) && (m_column == col);
}
