#include "wxc_aui_manager.h"

wxcAuiManager::wxcAuiManager() {}

wxcAuiManager::~wxcAuiManager() {}

wxcAuiManager& wxcAuiManager::Get()
{
    static wxcAuiManager mgr;
    return mgr;
}

void wxcAuiManager::UnInit(wxWindow* win)
{
    std::map<wxWindow*, wxAuiManager*>::iterator iter = m_auiMgrMap.find(win);
    if(iter != m_auiMgrMap.end()) {
        iter->second->UnInit();
        delete iter->second;
        m_auiMgrMap.erase(iter);
    }
}

void wxcAuiManager::Add(wxWindow* win, wxAuiManager* aui)
{
    wxASSERT_MSG(!m_auiMgrMap.count(win), "A wxWindow can have only 1 AUI manager!");
    aui->SetManagedWindow(win);
    m_auiMgrMap.insert(std::make_pair(win, aui));
}

wxAuiManager* wxcAuiManager::Find(wxWindow* win) const
{
    if(m_auiMgrMap.count(win) == 0) return NULL;
    return m_auiMgrMap.find(win)->second;
}
