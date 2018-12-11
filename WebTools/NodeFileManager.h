#ifndef NODEFILEMANAGER_H
#define NODEFILEMANAGER_H

#include "wxStringHash.h"
#include <wx/string.h>

class NodeFileManager
{
    std::unordered_map<wxString, wxString> m_files;
    std::unordered_map<wxString, wxString> m_remoteFiles;

protected:
    wxString DoGetFilePath(const wxString& id) const;

public:
    NodeFileManager();
    virtual ~NodeFileManager();
    static NodeFileManager& Get();

    void Clear();
    void AddFile(const wxString& id, const wxString& url);
    wxString GetFilePath(const wxString& id) const;
    /**
     * @brief is file represented by ID exists locally?
     */
    bool IsFileExists(const wxString& id) const;
    /**
     * @brief cache remote copy on the file system
     */
    void CacheRemoteCopy(const wxString& id, const wxString& fileContent);
    
    static wxString URIToFileName(const wxString& uri);
    static wxString FileNameToURI(const wxString& uri);
};

#endif // NODEFILEMANAGER_H
