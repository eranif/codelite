#include "build_custom_targets_menu_manager.h"
#include "macros.h"

CustomTargetsMgr::CustomTargetsMgr()
{
}

CustomTargetsMgr::~CustomTargetsMgr()
{
}

CustomTargetsMgr& CustomTargetsMgr::Get()
{
    static CustomTargetsMgr theMgr;
    return theMgr;
}

void CustomTargetsMgr::SetTargets(const wxString& projectName, const BuildConfig::StringMap_t& targets)
{
    Clear();
    
    m_projectName = projectName;
    BuildConfig::StringMap_t::const_iterator iter = targets.begin();
    
    int id = ID_MENU_CUSTOM_TARGET_FIRST;
    for(; iter != targets.end(); ++iter) {
        m_targets.insert( std::make_pair( id, std::make_pair(iter->first, iter->second) ) );
        ++id;
    }
}

CustomTargetsMgr::Pair_t CustomTargetsMgr::GetTarget(int menuId) const
{
    if ( m_targets.count(menuId) ) {
        return m_targets.find(menuId)->second;
    }
    return Pair_t();
}

void CustomTargetsMgr::Clear()
{
    m_projectName.Clear();
    m_targets.clear();
}

