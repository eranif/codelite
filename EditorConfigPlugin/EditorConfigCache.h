#ifndef EDITORCONFIGCACHE_H
#define EDITORCONFIGCACHE_H

#include <wx/filename.h>
#include "clEditorConfig.h"
#include <map>

class EditorConfigCache
{
private:
    struct CachedItem {
        clEditorConfigSection section;
        time_t lastModified;
    };
    
    std::map<wxString, CachedItem> m_map;

public:
    EditorConfigCache();
    virtual ~EditorConfigCache();

    /**
     * @brief find cached item for given filename
     */
    bool Get(const wxFileName& filename, clEditorConfigSection& section);
    
    /**
     * @brief add entry to the cache
     */
    void Add(const wxFileName& filename, const clEditorConfigSection& section);
    
    /**
     * @brief clear the cache
     */
    void Clear();
};

#endif // EDITORCONFIGCACHE_H
