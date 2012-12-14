#ifndef CLCONFIG_H
#define CLCONFIG_H

#include "codelite_exports.h"
#include "json_node.h"

class WXDLLIMPEXP_SDK clConfig
{
protected:
    wxFileName m_filename;
    JSONRoot* m_root;
    
public:
    clConfig();
    static clConfig& Get();
    
    // Workspace tab order
    // -----------------------------
    void SetWorkspaceTabOrder( const wxArrayString& tabs, int selected );
    bool GetWorkspaceTabOrder( wxArrayString& tabs, int &selected );
    
private:
    virtual ~clConfig();

};

#endif // CLCONFIG_H
