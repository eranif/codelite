#include "ServiceProvider.h"
#include "ServiceProviderManager.h"

ServiceProvider::ServiceProvider(const wxString& name, eServiceType type)
    : m_name(name), m_type(type)
{
    ServiceProviderManager::Get().Register(this);
}

ServiceProvider::~ServiceProvider()
{
    ServiceProviderManager::Get().Unregister(this);
}

