#ifndef OUTLINESETTINGS_H
#define OUTLINESETTINGS_H

#include <wx/string.h>
#include "json_node.h"

class OutlineSettings
{
    int m_tabIndex;
    
public:
    OutlineSettings();
    virtual ~OutlineSettings();

    void SetTabIndex(int tabIndex) {
        this->m_tabIndex = tabIndex;
    }
    int GetTabIndex() const {
        return m_tabIndex;
    }
    
    void Load();
    void Save();
};

#endif // OUTLINESETTINGS_H
