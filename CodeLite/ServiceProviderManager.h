#ifndef SERVICEPROVIDERMANAGER_H
#define SERVICEPROVIDERMANAGER_H

#include "codelite_exports.h"
#include "ServiceProvider.h"
#include <vector>
#include <unordered_map>
#include <wx/event.h>

namespace std
{
    template <>
    struct hash<eServiceType>
    {
        std::size_t operator()(const eServiceType& t) const { return static_cast<std::size_t>(t); }
    };
}

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
