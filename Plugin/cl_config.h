#ifndef CLCONFIG_H
#define CLCONFIG_H

#include "codelite_exports.h"
#include "json_node.h"

////////////////////////////////////////////////////////

class WXDLLIMPEXP_SDK clConfigItem
{
protected:
    wxString m_name;

public:
    clConfigItem(const wxString &name)
        : m_name(name)
    {}

    virtual ~clConfigItem()
    {}

    const wxString& GetName() const {
        return m_name;
    }
    
    virtual void FromJSON(const JSONElement& json) = 0;
    virtual JSONElement ToJSON() const = 0;
};



////////////////////////////////////////////////////////

class WXDLLIMPEXP_SDK clConfig
{
protected:
    wxFileName m_filename;
    JSONRoot* m_root;

protected:
    void DoDeleteProperty(const wxString &property);

public:
    // We provide a global configuration
    // and the ability to allocate a private copy with a different file
    clConfig(const wxString& filename = "codelite.conf");
    virtual ~clConfig();
    static clConfig& Get();
    
    // Re-read the content from the disk
    void Reload();
    
    // Workspace tab order
    // -----------------------------
    void SetWorkspaceTabOrder( const wxArrayString& tabs, int selected );
    bool GetWorkspaceTabOrder( wxArrayString& tabs, int &selected );
    
    // General objects
    // -----------------------------
    bool ReadItem(clConfigItem* item);
    void WriteItem(const clConfigItem* item);
    
};


#endif // CLCONFIG_H
