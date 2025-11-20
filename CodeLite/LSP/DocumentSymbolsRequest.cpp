#include "LSP/DocumentSymbolsRequest.hpp"

#include "LSP/LSPEvent.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "json_rpc_params.h"

#include <algorithm>
#include <thread>

namespace
{
clModuleLogger& operator<<(clModuleLogger& logger, const std::vector<LSP::SymbolInformation>& symbols)
{
    if (!logger.CanLog()) {
        return logger;
    }

    wxString s;
    s << "\n[\n";
    for (const LSP::SymbolInformation& d : symbols) {
        s << "  " << d.GetContainerName() << "." << d.GetName() << ",\n";
    }
    s << "]\n";
    logger.Append(s);
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

void LSP::DocumentSymbolsRequest::OnResponse(const LSP::ResponseMessage& const_response, wxEvtHandler* owner)
{
    LSP_DEBUG() << "LSP::DocumentSymbolsRequest::OnResponse() is called!" << endl;
    LSP::ResponseMessage& response = const_cast<LSP::ResponseMessage&>(const_response);
    auto json = response.take();
    if (!json->toElement().hasNamedObject("result")) {
        LSP_WARNING() << "LSP::DocumentSymbolsRequest::OnResponse(): invalid 'result' object";
        return;
    }

    auto result = json->toElement().namedObject("result");
        if (!result.isArray() || result.arraySize() == 0) {            
        // Nothing to display
        return;
    }

    int size = result.arraySize();
    wxString filename = m_params->As<DocumentSymbolParams>()->GetTextDocument().GetPath();
    auto context = m_context;
    // ToDo: is selectionRange really the best distinction between DocumentSymbol and SymbolInformation?
    if (result[0].hasNamedObject("selectionRange")) { 
        std::vector<LSP::DocumentSymbol> symbols;
        symbols.reserve(size);
        wxString debug_str;
        for (int i = 0; i < size; ++i) {
            DocumentSymbol ds;
            ds.FromJSON(result[i]);
            symbols.push_back(ds);
            debug_str += ds.ToString();
        }
        
        LSP_DEBUG() << "Received DocumentSymbol: " << endl << debug_str << endl;
        return;
    }
    else if (result[0].hasNamedObject("location")) {
        std::vector<LSP::SymbolInformation> symbols;
        symbols.reserve(size);
        for (int i = 0; i < size; ++i) {
            SymbolInformation si;
            si.FromJSON(result[i]);
            symbols.push_back(si);
        }

        // sort the items by line position
        std::sort(symbols.begin(),
                  symbols.end(),
                  [=](const LSP::SymbolInformation& a, const LSP::SymbolInformation& b) -> int {
                      return a.GetLocation().GetRange().GetStart().GetLine() <
                             b.GetLocation().GetRange().GetStart().GetLine();
                  });

        LOG_IF_TRACE { LSP_TRACE() << symbols << endl; }

        // fire event per context
        if (context & CONTEXT_SEMANTIC_HIGHLIGHT) {
            QueueEvent(owner, symbols, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_FOR_HIGHLIGHT);
        }

        bool outline_event_fired_for_event_notifier{false};
        if (context & CONTEXT_OUTLINE_VIEW) {
            QueueEvent(owner, symbols, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW);
            // if "owner" is "EventNotifier::Get()" do not send the same event twice.
            outline_event_fired_for_event_notifier = (owner == EventNotifier::Get());
        }

        if (!outline_event_fired_for_event_notifier) {
            // always fire the wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW for the EventNotifier
            // so it might be used by other plugins as well, e.g. "Outline"
            QueueEvent(EventNotifier::Get(), symbols, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW);
        }

        InvokeResponseCallback(CreateLSPEvent(symbols, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_QUICK_OUTLINE));
    }
}

LSPEvent LSP::DocumentSymbolsRequest::CreateLSPEvent(const std::vector<LSP::SymbolInformation>& symbols,
                                                     const wxString& filename,
                                                     const wxEventType& event_type)
{
    LSPEvent event{event_type};
    event.GetSymbolsInformation().reserve(symbols.size());
    event.GetSymbolsInformation().insert(event.GetSymbolsInformation().end(), symbols.begin(), symbols.end());
    event.SetFileName(filename);
    return event;
}

void LSP::DocumentSymbolsRequest::QueueEvent(wxEvtHandler* owner,
                                             const std::vector<LSP::SymbolInformation>& symbols,
                                             const wxString& filename,
                                             const wxEventType& event_type)
{
    LSPEvent event = CreateLSPEvent(symbols, filename, event_type);
    owner->QueueEvent(event.Clone());
}
