#include "ServiceProvider.h"
#include "ServiceProviderManager.h"

ServiceProvider::ServiceProvider(const wxString& name, eServiceType type)
    : m_name(name)
    , m_type(type)
{
    ServiceProviderManager::Get().Register(this);
}

ServiceProvider::~ServiceProvider() { ServiceProviderManager::Get().Unregister(this); }

void ServiceProvider::SetPriority(int priority)
{
    m_priority = priority;
    ServiceProviderManager::Get().Sort(GetType());
}
