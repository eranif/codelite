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

    wxString filename = m_params->As<DocumentSymbolParams>()->GetTextDocument().GetPath();
    auto context = m_context;
    
    auto result = json->toElement().namedObject("result");
    
    // vectors to store the parsed result
    std::vector<LSP::DocumentSymbol> documentSymbols;
    std::vector<LSP::SymbolInformation> symbolsInformation;
    
    if (result.isArray() && result.arraySize() > 0) {   
        // fill our result vectors with data from json result
        int size = result.arraySize();
        // response can either be DocumentSymbol[] or SymbolInformation[]
        // try to determine which one we got
        if (result[0].hasNamedObject("detail") || result[0].hasNamedObject("selectionRange")) { 
            std::vector<DocumentSymbol> containers;            
            // parse json
            for (int i = 0; i < size; ++i) {
                DocumentSymbol ds;
                ds.FromJSON(result[i]);
                
                // sort into containers
                if (!ds.GetContainer().empty()) {
                    auto iContainer = std::find_if(containers.begin(), containers.end(), 
                        [&ds](const DocumentSymbol& s) { return s.GetName() == ds.GetContainer(); });
                    if (iContainer == containers.end()) {
                        // create new container
                        DocumentSymbol container;
                        container.SetName(ds.GetContainer());
                        container.SetKind(eSymbolKind::kSK_Container);
                        container.SetRange(ds.GetRange());
                        container.GetChildren().push_back(ds);
                        containers.push_back(container);
                    }
                    else {
                        // insert into existing container and adjust it's range
                        iContainer->GetChildren().push_back(ds);                     
                        iContainer->GrowContainerRange(ds);              
                    }
                }
                else {
                    // symbol name doesn't contain a container (::), so add it directly to the root
                    documentSymbols.push_back(ds);
                }
            }
            documentSymbols.insert(documentSymbols.end(), containers.begin(), containers.end());
        }
        else if (result[0].hasNamedObject("location")) {
            symbolsInformation.reserve(size);
            for (int i = 0; i < size; ++i) {
                SymbolInformation si;
                si.FromJSON(result[i]);
                symbolsInformation.push_back(si);
            }

            // sort the items by line position
            std::sort(symbolsInformation.begin(),
                      symbolsInformation.end(),
                      [=](const LSP::SymbolInformation& a, const LSP::SymbolInformation& b) -> int {
                          return a.GetLocation().GetRange().GetStart().GetLine() <
                                 b.GetLocation().GetRange().GetStart().GetLine();
                      });

            LOG_IF_TRACE { LSP_TRACE() << symbolsInformation << endl; }

        }
    }
    
    // fire event per context
    if (context & CONTEXT_SEMANTIC_HIGHLIGHT) {
        QueueEvent(owner, documentSymbols, symbolsInformation, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_FOR_HIGHLIGHT);
    }

    bool outline_event_fired_for_event_notifier{false};
    if (context & CONTEXT_OUTLINE_VIEW) {
        QueueEvent(owner, documentSymbols, symbolsInformation, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW);
        // if "owner" is "EventNotifier::Get()" do not send the same event twice.
        outline_event_fired_for_event_notifier = (owner == EventNotifier::Get());
    }

    if (!outline_event_fired_for_event_notifier) {
        // always fire the wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW for the EventNotifier
        // so it might be used by other plugins as well, e.g. "Outline"
        QueueEvent(EventNotifier::Get(), documentSymbols, symbolsInformation, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW);
    }

    InvokeResponseCallback(CreateLSPEvent(documentSymbols, symbolsInformation, filename, wxEVT_LSP_DOCUMENT_SYMBOLS_QUICK_OUTLINE));
}

LSPEvent LSP::DocumentSymbolsRequest::CreateLSPEvent(const std::vector<LSP::DocumentSymbol>& documentSymbols,
                                                     const std::vector<LSP::SymbolInformation>& symbolsInformation,
                                                     const wxString& filename,
                                                     const wxEventType& event_type)
{
    LSPEvent event{event_type};
    if (!documentSymbols.empty()) {        
        event.GetDocumentSymbols().reserve(documentSymbols.size());
        event.GetDocumentSymbols().insert(event.GetDocumentSymbols().end(), documentSymbols.begin(), documentSymbols.end());
    }
    else if (!symbolsInformation.empty()) {        
        event.GetSymbolsInformation().reserve(symbolsInformation.size());
        event.GetSymbolsInformation().insert(event.GetSymbolsInformation().end(), symbolsInformation.begin(), symbolsInformation.end());
    }
    event.SetFileName(filename);
    return event;
}

void LSP::DocumentSymbolsRequest::QueueEvent(wxEvtHandler* owner,
                                             const std::vector<LSP::DocumentSymbol>& documentSymbols,
                                             const std::vector<LSP::SymbolInformation>& symbolsInformation,
                                             const wxString& filename,
                                             const wxEventType& event_type)
{
    LSPEvent event = CreateLSPEvent(documentSymbols, symbolsInformation, filename, event_type);
    owner->QueueEvent(event.Clone());
}