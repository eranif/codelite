#ifndef BUILDCUSTOMTARGETSMENUMANAGER_H
#define BUILDCUSTOMTARGETSMENUMANAGER_H

#include <wx/string.h>
#include <map>
#include "build_config.h"

class CustomTargetsMgr
{
public:
    typedef std::pair<wxString, wxString> Pair_t;
    typedef std::map<int, Pair_t>         Map_t;

protected:
    Map_t    m_targets;
    wxString m_projectName;

public:
    CustomTargetsMgr();
    virtual ~CustomTargetsMgr();
    void Clear();
    
    static CustomTargetsMgr& Get();
    void SetTargets( const wxString &projectName, const BuildConfig::StringMap_t& targets );
    CustomTargetsMgr::Pair_t GetTarget(int menuId) const;
    const CustomTargetsMgr::Map_t& GetTargets() const {
        return m_targets;
    }
    const wxString& GetProjectName() const {
        return m_projectName;
    }
};

#endif // BUILDCUSTOMTARGETSMENUMANAGER_H
