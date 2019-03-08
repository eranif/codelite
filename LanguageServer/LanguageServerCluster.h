#ifndef LANGUAGESERVERCLUSTER_H
#define LANGUAGESERVERCLUSTER_H

#include <wx/event.h>
#include "LanguageServerProtocol.h"
#include <wxStringHash.h>
#include "cl_command_event.h"
#include <wx/sharedptr.h>
#include "LSP/LSPEvent.h"

class LanguageServerCluster : public wxEvtHandler
{
    std::unordered_map<wxString, LanguageServerProtocol::Ptr_t> m_servers;

public:
    typedef wxSharedPtr<LanguageServerCluster> Ptr_t;

protected:
    LanguageServerProtocol::Ptr_t GetServerForFile(const wxFileName& filename);

    void OnSymbolFound(LSPEvent& event);

public:
    LanguageServerCluster();
    virtual ~LanguageServerCluster();

    void Reload();
    void OnFindSymbold(clCodeCompletionEvent& event);
};

#endif // LANGUAGESERVERCLUSTER_H
