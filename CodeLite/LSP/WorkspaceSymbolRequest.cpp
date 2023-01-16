#include "WorkspaceSymbolRequest.hpp"

#include "LSPEvent.h"
#include "event_notifier.h"
#include "file_logger.h"

namespace
{
clModuleLogger& operator<<(clModuleLogger& logger, const std::vector<LSP::SymbolInformation>& symbols)
{
    if(!logger.CanLog())
        return logger;

    wxString s;
    s << "\n[\n";
    for(const LSP::SymbolInformation& d : symbols) {
        s << "  " << d.GetContainerName() << "." << d.GetName() << ",\n";
    }
    s << "]\n";
    logger.Append(s);
    return logger;
}
} // namespace

namespace LSP
{
class WorkspaceSymbolParams : public Params
{
    wxString m_query;

public:
    WorkspaceSymbolParams() {}
    virtual ~WorkspaceSymbolParams() {}

    virtual void FromJSON(const JSONItem& json) { m_query = json["query"].toString(); }

    virtual JSONItem ToJSON(const wxString& name) const
    {
        JSONItem json = JSONItem::createObject(name);
        json.addProperty("query", m_query);
        return json;
    }

    void SetQuery(const wxString& query) { this->m_query = query; }
    const wxString& GetQuery() const { return this->m_query; }
};
} // namespace LSP

LSP::WorkspaceSymbolRequest::WorkspaceSymbolRequest(const wxString& query)
{
    SetMethod("workspace/symbol");
    // set the params
    m_params.reset(new WorkspaceSymbolParams());
    m_params->As<WorkspaceSymbolParams>()->SetQuery(query);
}

LSP::WorkspaceSymbolRequest::~WorkspaceSymbolRequest() {}

void LSP::WorkspaceSymbolRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    wxUnusedVar(owner);

    auto result = response.Get("result");
    if(!result.isOk()) {
        LSP_WARNING() << "LSP::WorkspaceSymbolRequest::OnResponse(): invalid 'result' object";
        return;
    }
    if(!result.isArray()) {
        LSP_WARNING() << "workspace/symbol: expected array result" << endl;
        return;
    }

    int size = result.arraySize();
    if(size == 0) {
        LSPEvent symbols_event{ wxEVT_LSP_WORKSPACE_SYMBOLS };
        owner->QueueEvent(symbols_event.Clone());
        return;
    }

    LOG_IF_TRACE { LSP_TRACE() << result.format() << endl; }
    // only SymbolInformation has the `location` property
    // fire an event with all the symbols
    LSPEvent symbols_event{ wxEVT_LSP_WORKSPACE_SYMBOLS };
    auto& symbols = symbols_event.GetSymbolsInformation();
    symbols.reserve(size);

    for(int i = 0; i < size; ++i) {
        SymbolInformation si;
        si.FromJSON(result[i]);
        symbols.push_back(si);
    }

    LOG_IF_TRACE { LSP_TRACE() << symbols_event.GetSymbolsInformation() << endl; }
    EventNotifier::Get()->QueueEvent(symbols_event.Clone());
}
