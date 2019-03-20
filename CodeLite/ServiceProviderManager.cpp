#include "ServiceProviderManager.h"
#include <algorithm>
#include "event_notifier.h"
#include "codelite_events.h"
#include "file_logger.h"

ServiceProviderManager::ServiceProviderManager() {}

ServiceProviderManager::~ServiceProviderManager() { m_providers.clear(); }

void ServiceProviderManager::Register(ServiceProvider* provider)
{
    // No duplicates
    Unregister(provider);
    // Now register the service
    if(this->m_providers.count(provider->GetType()) == 0) {
        m_providers.insert({ provider->GetType(), ServiceProvider::Vec_t() });
    }
    ServiceProvider::Vec_t& V = m_providers[provider->GetType()];
    V.push_back(provider);

    clDEBUG() << "Handler:" << provider->GetName() << "registerd. Priority:" << provider->GetPriority()
              << ". Type:" << (int)provider->GetType();

    // Sort the providers by priority (descending order)
    std::sort(V.begin(), V.end(),
              [](ServiceProvider* a, ServiceProvider* b) { return a->GetPriority() > b->GetPriority(); });

    wxString order;
    for(ServiceProvider* p : V) {
        order << p->GetName() << "(" << p->GetPriority() << ") ";
    }
    clDEBUG() << "Service providers:" << order;
}

void ServiceProviderManager::Unregister(ServiceProvider* provider)
{
    if(this->m_providers.count(provider->GetType()) == 0) { return; }
    ServiceProvider::Vec_t& V = m_providers[provider->GetType()];

    // Find our provider and remove it
    while(true) {
        // Do this in a loop, incase someone registered this provider twice...
        auto where =
            std::find_if(V.begin(), V.end(), [&](ServiceProvider* p) { return p->GetName() == provider->GetName(); });
        if(where == V.end()) { break; }
        V.erase(where); // remove it
        clDEBUG() << "Handler:" << provider->GetName() << "Uregisterd. Priority:" << provider->GetPriority()
                  << ". Type:" << (int)provider->GetType();
    }
}

ServiceProviderManager& ServiceProviderManager::Get()
{
    static ServiceProviderManager instance;
    return instance;
}

bool ServiceProviderManager::ProcessEvent(wxEvent& event)
{
    eServiceType type = GetServiceFromEvent(event);
    if(type == eServiceType::kUnknown || m_providers.count(type) == 0) {
        // Let the default event processing take place by using EventNotifier
        return EventNotifier::Get()->ProcessEvent(event);
    } else {
        // Call our chain
        auto& V = m_providers[type];
        for(ServiceProvider* p : V) {
            if(p->ProcessEvent(event)) { return true; }
        }
        return false;
    }
}

eServiceType ServiceProviderManager::GetServiceFromEvent(wxEvent& event)
{
    wxEventType type = event.GetEventType();
    // Code Completion events
    if(type == wxEVT_CC_CODE_COMPLETE || type == wxEVT_CC_FIND_SYMBOL || type == wxEVT_CC_FIND_SYMBOL_DECLARATION ||
       type == wxEVT_CC_FIND_SYMBOL_DEFINITION || type == wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP ||
       type == wxEVT_CC_TYPEINFO_TIP || type == wxEVT_CC_WORD_COMPLETE) {
        return eServiceType::kCodeCompletion;
    }
    return eServiceType::kUnknown;
}

void ServiceProviderManager::UnregisterAll() { m_providers.clear(); }

void ServiceProviderManager::Sort(eServiceType type)
{
    if(m_providers.count(type) == 0) { return; }
    clDEBUG() << "sorting providers for type:" << (int)type;
    ServiceProvider::Vec_t& V = m_providers[type];
    std::sort(V.begin(), V.end(),
              [](ServiceProvider* a, ServiceProvider* b) { return a->GetPriority() > b->GetPriority(); });
    wxString order;
    for(ServiceProvider* p : V) {
        order << p->GetName() << "(" << p->GetPriority() << ") ";
    }
    clDEBUG() << "Service providers:" << order;
}
