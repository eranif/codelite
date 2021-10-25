#ifndef SERVICEPROVIDERMANAGER_H
#define SERVICEPROVIDERMANAGER_H

#include "ServiceProvider.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include <unordered_map>
#include <vector>
#include <wx/event.h>

namespace std
{
template <> struct hash<eServiceType> {
    std::size_t operator()(const eServiceType& t) const { return static_cast<std::size_t>(t); }
};
} // namespace std

// Similar to EventNotifier class.
// But process events in order. Each 'handler' (aka: ServiceProvider)
// Registers itself for a service with a priority. The higher the prio, it will get
// called first
class WXDLLIMPEXP_CL ServiceProviderManager : public wxEvtHandler
{
    std::unordered_map<eServiceType, ServiceProvider::Vec_t> m_providers;
    friend class ServiceProvider;

protected:
    ServiceProviderManager();
    virtual ~ServiceProviderManager();

    void Register(ServiceProvider* provider);
    void Unregister(ServiceProvider* provider);
    eServiceType GetServiceFromEvent(wxEvent& event);

    void OnActiveEditorChanged(wxCommandEvent& event);
    void OnEditorSaved(clCommandEvent& event);
    void RequestSemanticsHighlights(const wxString& filename);

public:
    static ServiceProviderManager& Get();

    /**
     * @brief remove all providers
     */
    void UnregisterAll();

    /**
     * @brief process service event
     */
    virtual bool ProcessEvent(wxEvent& event);

    /**
     * @brief sort the service providers for a given type
     */
    void Sort(eServiceType type);
};
#endif // SERVICEPROVIDERMANAGER_H
