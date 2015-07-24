#ifndef NODEJSWORKSPACECONFIGURATION_H
#define NODEJSWORKSPACECONFIGURATION_H

#include "cl_config.h"
#include <wx/arrstr.h>

class NodeJSWorkspaceConfiguration : public clConfigItem
{
    wxArrayString m_folders;
    bool m_isOk;
    bool m_showHiddenFiles;

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    NodeJSWorkspaceConfiguration();
    virtual ~NodeJSWorkspaceConfiguration();

    NodeJSWorkspaceConfiguration& Load(const wxFileName& filename);
    NodeJSWorkspaceConfiguration& Save(const wxFileName& filename);
    NodeJSWorkspaceConfiguration& SetFolders(const wxArrayString& folders)
    {
        this->m_folders = folders;
        return *this;
    }

    NodeJSWorkspaceConfiguration& SetIsOk(bool isOk)
    {
        this->m_isOk = isOk;
        return *this;
    }
    NodeJSWorkspaceConfiguration& SetShowHiddenFiles(bool showHiddenFiles)
    {
        this->m_showHiddenFiles = showHiddenFiles;
        return *this;
    }
    bool IsShowHiddenFiles() const { return m_showHiddenFiles; }
    const wxArrayString& GetFolders() const { return m_folders; }
    bool IsOk() const { return m_isOk; }
};

#endif // NODEJSWORKSPACECONFIGURATION_H
