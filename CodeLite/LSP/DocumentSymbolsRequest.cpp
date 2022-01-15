#include "LSP/DocumentSymbolsRequest.hpp"

#include "LSP/LSPEvent.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "json_rpc_params.h"

#include <algorithm>
#include <thread>

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

LSP::DocumentSymbolsRequest::DocumentSymbolsRequest(const wxString& filename, size_t context)
    : m_context(context)
{
    SetMethod("textDocument/documentSymbol");
    // set the params
    m_params.reset(new DocumentSymbolParams());
    m_params->As<DocumentSymbolParams>()->SetTextDocument(TextDocumentIdentifier(filename));
}

LSP::DocumentSymbolsRequest::~DocumentSymbolsRequest() {}

void LSP::DocumentSymbolsRequest::OnResponse(const LSP::ResponseMessage& const_response, wxEvtHandler* owner)
{
    clDEBUG() << "LSP::DocumentSymbolsRequest::OnResponse() is called!" << endl;
    LSP::ResponseMessage& response = const_cast<LSP::ResponseMessage&>(const_response);
    auto json = std::move(response.take());
    if(!json->toElement().hasNamedObject("result")) {
        clWARNING() << "LSP::DocumentSymbolsRequest::OnResponse(): invalid 'result' object";
        return;
    }

    auto result = json->toElement().namedObject("result");
    if(result.isArray()) {
        int size = result.arraySize();
        if(size == 0) {
            return;
        }

        wxString filename = m_params->As<DocumentSymbolParams>()->GetTextDocument().GetPath();
        auto context = m_context;
        if(result[0].hasNamedObject("location")) {
            auto result = json->toElement().namedObject("result");
            std::vector<LSP::SymbolInformation> symbols;
            symbols.reserve(size);
            for(int i = 0; i < size; ++i) {
                SymbolInformation si;
                si.FromJSON(result[i]);
                symbols.push_back(si);
            }

            // sort the items by line position
            std::sort(symbols.begin(), symbols.end(),
                      [=](const LSP::SymbolInformation& a, const LSP::SymbolInformation& b) -> int {
                          return a.GetLocation().GetRange().GetStart().GetLine() <
                                 b.GetLocation().GetRange().GetStart().GetLine();
                      });
            clDEBUG1() << symbols << endl;

            // fire event per context
            if(context & CONTEXT_SEMANTIC_HIGHLIGHT) {
                QueueEvent(owner, symbols, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_FOR_HIGHLIGHT);
            }
            if(context & CONTEXT_QUICK_OUTLINE) {
                QueueEvent(owner, symbols, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_QUICK_OUTLINE);
            }
            if(context & CONTEXT_OUTLINE_VIEW) {
                QueueEvent(owner, symbols, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW);
            }

            // always fire the wxEVT_LSP_DOCUMENT_SYMBOLS_QUICK_OUTLINE for the EventNotifier
            // so it might be used by other plugins as well, e.g. "Outline"
            QueueEvent(EventNotifier::Get(), symbols, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_QUICK_OUTLINE);
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

void LSP::DocumentSymbolsRequest::QueueEvent(wxEvtHandler* owner, const std::vector<LSP::SymbolInformation>& symbols,
                                             const wxString& filename, const wxEventType& event_type)
{
    LSPEvent event{ event_type };
    event.GetSymbolsInformation().reserve(symbols.size());
    event.GetSymbolsInformation().insert(event.GetSymbolsInformation().end(), symbols.begin(), symbols.end());
    event.SetFileName(filename);
    owner->QueueEvent(event.Clone());
}
