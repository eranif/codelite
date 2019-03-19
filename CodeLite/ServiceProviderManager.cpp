#include "ServiceProviderManager.h"
#include <algorithm>

ServiceProviderManager::ServiceProviderManager() {}

ServiceProviderManager::~ServiceProviderManager() {}

void ServiceProviderManager::Register(ServiceProvider* provider)
{
    if(this->m_providers.count(provider->GetType()) == 0) {
        m_providers.insert({ provider->GetType(), ServiceProvider::Vec_t() });
    }
    ServiceProvider::Vec_t& V = m_providers[provider->GetType()];
    V.push_back(provider);

    // Sort the providers by priority (descending order)
    std::sort(V.begin(), V.end(),
              [](ServiceProvider* a, ServiceProvider* b) { return a->GetPriority() > b->GetPriority(); });
}

void ServiceProviderManager::Unregister(ServiceProvider* provider)
{
    if(this->m_providers.count(provider->GetType()) == 0) { return; }
    ServiceProvider::Vec_t& V = m_providers[provider->GetType()];

    // Find our provider and remove it
    while(true) {
        // Do this in a loop, incase someone registered this provider twice...
        auto where = std::find_if(V.begin(), V.end(), [&](ServiceProvider* p) {
            return p == provider; // yes, pointer comparison...
        });
        if(where == V.end()) { break; }
        V.erase(where); // remove it
    }
}

ServiceProviderManager& ServiceProviderManager::Get()
{
    thread_local ServiceProviderManager instance;
    return instance;
}

bool ServiceProviderManager::ProcessEvent(wxEvent& event, eServiceType type)
{
    if(m_providers.count(type)) {
        auto& V = m_providers[type];
        for(ServiceProvider* p : V) {
            if(p->ProcessEvent(event)) { return true; }
        }
        return false;
    } else {
        return wxEvtHandler::ProcessEvent(event);
    }
}
